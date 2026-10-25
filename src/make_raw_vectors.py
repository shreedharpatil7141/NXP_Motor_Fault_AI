from __future__ import annotations

from pathlib import Path

import numpy as np
import tensorflow as tf


ROOT = Path(__file__).resolve().parents[1]
RAW_DIR = ROOT / "dataset" / "raw" / "CWRU_Bearing_NumPy"
MODELS_DIR = ROOT / "models"
OUTPUT = ROOT / "motor_fault_npu" / "raw_vectors.h"
WINDOW = 1024
STRIDE = 512
RPM = 1772
WINDOWS_PER_CLASS = 5
SEED = 42
INPUT_SCALE = 0.02456347830593586
INPUT_ZERO_POINT = -78


def label_for(path: Path) -> int:
    if "Normal" in path.name:
        return 0
    if "_IR" in path.name:
        return 1
    if "_OR@" in path.name:
        return 2
    if "_B" in path.name:
        return 3
    raise ValueError(f"Unsupported class in {path.name}")


def load_signal(path: Path) -> np.ndarray:
    with np.load(path) as data:
        if "DE" not in data:
            raise KeyError(f"{path} does not contain DE")
        return np.asarray(data["DE"], dtype=np.float32).reshape(-1)


def windows_for(path: Path) -> np.ndarray:
    signal = load_signal(path)
    starts = range(0, len(signal) - WINDOW + 1, STRIDE)
    return np.asarray(
        [signal[start : start + WINDOW] for start in starts], dtype=np.float32
    )


def quantize_features(features: np.ndarray) -> np.ndarray:
    normalized = (features - np.float32(stats["mean"])) / np.float32(stats["std"])
    quantized = np.rint(normalized / INPUT_SCALE + INPUT_ZERO_POINT)
    return np.clip(quantized, -128, 127).astype(np.int8)


def format_values(values: np.ndarray, per_line: int = 16) -> str:
    flat = values.reshape(-1)
    lines = []
    for start in range(0, len(flat), per_line):
        lines.append("    " + ", ".join(str(x) for x in flat[start : start + per_line]))
    return ",\n".join(lines)


def format_float_values(values: np.ndarray, per_line: int = 8) -> str:
    flat = values.reshape(-1)
    lines = []
    for start in range(0, len(flat), per_line):
        lines.append(
            "    "
            + ", ".join(
                (
                    f"{float(x):.9g}"
                    if any(ch in f"{float(x):.9g}" for ch in ".eE")
                    else f"{float(x):.9g}.0"
                )
                + "f"
                for x in flat[start : start + per_line]
            )
        )
    return ",\n".join(lines)


def format_debug_table(window0: np.ndarray, sine: np.ndarray) -> str:
    window0_mag = np.abs(np.fft.rfft(window0))[:8]
    sine_mag = np.abs(np.fft.rfft(sine))[49:52]
    return (
        "static const float raw_debug_numpy_window0_mag[8] = {\n"
        f"{format_float_values(window0_mag)}\n"
        "};\n\n"
        "static const float raw_debug_numpy_sine_mag[3] = {\n"
        f"{format_float_values(sine_mag)}\n"
        "};"
    )


def main() -> None:
    global stats
    stats = np.load(MODELS_DIR / "4class_norm_stats.npz")
    mean = float(stats["mean"])
    std = float(stats["std"])
    print(f"Normalization mean: {mean:.9g}")
    print(f"Normalization std: {std:.9g}")
    files = sorted((RAW_DIR / "Data" / f"{RPM} RPM").glob("*.npz"))
    files = [
        path
        for path in files
        if path.name.endswith("_DE12.npz") or path.name.endswith("_Normal.npz")
    ]
    by_class: dict[int, list[np.ndarray]] = {0: [], 1: [], 2: [], 3: []}
    for path in files:
        by_class[label_for(path)].append(windows_for(path))

    rng = np.random.default_rng(SEED)
    raw_parts = []
    labels = []
    for label in range(4):
        class_windows = np.concatenate(by_class[label], axis=0)
        if len(class_windows) < WINDOWS_PER_CLASS:
            raise ValueError(f"Class {label} has only {len(class_windows)} windows")
        selected = rng.choice(len(class_windows), WINDOWS_PER_CLASS, replace=False)
        raw_parts.append(class_windows[selected])
        labels.extend([label] * WINDOWS_PER_CLASS)

    raw = np.concatenate(raw_parts, axis=0)
    sine = 1000.0 * np.sin(2.0 * np.pi * 50.0 * np.arange(WINDOW) / WINDOW)
    features = np.log1p(np.abs(np.fft.rfft(raw, axis=1))).astype(np.float32)
    quantized_features = quantize_features(features)

    interpreter = tf.lite.Interpreter(
        model_path=str(MODELS_DIR / "4class_fault_classifier_int8.tflite")
    )
    interpreter.allocate_tensors()
    input_detail = interpreter.get_input_details()[0]
    output_detail = interpreter.get_output_details()[0]
    if input_detail["quantization"] != (INPUT_SCALE, INPUT_ZERO_POINT):
        raise ValueError(f"Unexpected input quantization: {input_detail['quantization']}")
    outputs = []
    for feature in quantized_features:
        interpreter.set_tensor(input_detail["index"], feature.reshape(1, 513, 1))
        interpreter.invoke()
        outputs.append(interpreter.get_tensor(output_detail["index"])[0])
    expected_out = np.asarray(outputs, dtype=np.int8)

    header = f"""#ifndef MOTOR_FAULT_RAW_VECTORS_H
#define MOTOR_FAULT_RAW_VECTORS_H

#include <stdint.h>

{format_debug_table(raw[0], sine)}

#define RAW_NUM_WINDOWS 20
#define RAW_WINDOW_SIZE 1024
#define RAW_FEATURE_SIZE 513

static const float raw_windows[20][1024] = {{
{format_float_values(raw)}
}};

static const uint8_t raw_labels[20] = {{
    {", ".join(str(x) for x in labels)}
}};

static const int8_t raw_expected_out[20][4] = {{
{format_values(expected_out)}
}};

static const int8_t raw_expected_feat[20][513] = {{
{format_values(quantized_features)}
}};

#endif
"""
    OUTPUT.write_text(header, encoding="ascii")
    print(f"Saved {OUTPUT}")
    print(f"Raw shape: {raw.shape}, feature shape: {quantized_features.shape}")


if __name__ == "__main__":
    main()
