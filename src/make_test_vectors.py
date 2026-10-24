"""Generate deterministic host/NPU comparison vectors for the motor-fault model."""

from __future__ import annotations

from pathlib import Path

import numpy as np
import tensorflow as tf


ROOT = Path(__file__).resolve().parents[1]
DATASET_PATH = ROOT / "dataset" / "processed" / "cwru_fft_4class.npz"
STATS_PATH = ROOT / "models" / "4class_norm_stats.npz"
MODEL_PATH = ROOT / "models" / "4class_fault_classifier_int8.tflite"
HEADER_PATH = ROOT / "motor_fault_npu" / "test_vectors.h"

HELD_OUT_RPM = 1772
NUM_PER_CLASS = 10
NUM_BINS = 513
NUM_CLASSES = 4
SEED = 42


def select_indices(y: np.ndarray, groups: np.ndarray, rpm: np.ndarray) -> np.ndarray:
    """Select balanced held-out-RPM samples, preferring distinct source groups."""
    rng = np.random.default_rng(SEED)
    selected: list[int] = []

    for label in range(NUM_CLASSES):
        candidates = np.flatnonzero((rpm == HELD_OUT_RPM) & (y == label))
        if len(candidates) < NUM_PER_CLASS:
            raise ValueError(
                f"Class {label} has {len(candidates)} held-out samples; "
                f"{NUM_PER_CLASS} are required."
            )

        shuffled = candidates[rng.permutation(len(candidates))]
        chosen: list[int] = []
        chosen_groups: set[object] = set()
        for index in shuffled:
            group = groups[index].item()
            if group not in chosen_groups:
                chosen.append(int(index))
                chosen_groups.add(group)
                if len(chosen) == NUM_PER_CLASS:
                    break

        if len(chosen) < NUM_PER_CLASS:
            chosen_set = set(chosen)
            chosen.extend(
                int(index)
                for index in shuffled
                if int(index) not in chosen_set
            )
            chosen = chosen[:NUM_PER_CLASS]
        selected.extend(chosen)

    return np.asarray(selected, dtype=np.int64)


def format_array(values: np.ndarray, values_per_line: int = 16) -> str:
    flat = values.reshape(-1)
    lines = []
    for start in range(0, len(flat), values_per_line):
        lines.append("    " + ", ".join(str(int(value)) for value in flat[start : start + values_per_line]))
    return ",\n".join(lines)


def main() -> None:
    dataset = np.load(DATASET_PATH)
    X = dataset["X"].astype(np.float32)
    y = dataset["y"].astype(np.uint8)
    groups = dataset["groups"]
    rpm = dataset["rpm"]
    if X.ndim != 2 or X.shape[1] != NUM_BINS:
        raise ValueError(f"Expected X shape (N, {NUM_BINS}), got {X.shape}.")

    stats = np.load(STATS_PATH)
    mean = np.float32(stats["mean"].item())
    std = np.float32(stats["std"].item())
    if std <= 0:
        raise ValueError("Normalization standard deviation must be positive.")

    indices = select_indices(y, groups, rpm)
    normalized = ((X[indices] - mean) / std).astype(np.float32)

    interpreter = tf.lite.Interpreter(model_path=str(MODEL_PATH))
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()[0]
    output_details = interpreter.get_output_details()[0]
    input_scale, input_zero_point = input_details["quantization"]
    if input_details["dtype"] is not np.int8 or input_scale <= 0:
        raise ValueError(
            f"Expected int8 input with positive scale, got "
            f"{input_details['dtype']} and quantization {input_details['quantization']}."
        )

    quantized = np.clip(
        np.rint(normalized / input_scale + input_zero_point), -128, 127
    ).astype(np.int8)
    expected_out = np.empty((len(indices), NUM_CLASSES), dtype=np.int8)
    predictions = np.empty(len(indices), dtype=np.uint8)

    for vector_index, vector in enumerate(quantized):
        input_value = vector.reshape(input_details["shape"]).astype(np.int8)
        interpreter.set_tensor(input_details["index"], input_value)
        interpreter.invoke()
        output = interpreter.get_tensor(output_details["index"]).reshape(-1)
        if output.size != NUM_CLASSES or output.dtype != np.int8:
            raise ValueError(
                f"Expected {NUM_CLASSES} int8 outputs, got {output.shape} {output.dtype}."
            )
        expected_out[vector_index] = output
        predictions[vector_index] = np.argmax(output)

    labels = y[indices]
    header = f"""#ifndef MOTOR_FAULT_TEST_VECTORS_H
#define MOTOR_FAULT_TEST_VECTORS_H

#include <stdint.h>

#define NUM_TEST_VECTORS {len(indices)}
#define NUM_BINS {NUM_BINS}

static const int8_t test_inputs[NUM_TEST_VECTORS][NUM_BINS] = {{
{format_array(quantized)}
}};

static const int8_t test_expected_out[NUM_TEST_VECTORS][4] = {{
{format_array(expected_out, 4)}
}};

static const uint8_t test_labels[NUM_TEST_VECTORS] = {{
{format_array(labels, 16)}
}};

#endif
"""
    HEADER_PATH.write_text(header, encoding="ascii")

    accuracy = float(np.mean(predictions == labels))
    print(f"Wrote {len(indices)} vectors to {HEADER_PATH}")
    print(f"Python-side INT8 accuracy: {accuracy:.4f} ({np.sum(predictions == labels)}/{len(labels)})")


if __name__ == "__main__":
    main()
