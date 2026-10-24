from __future__ import annotations

import numpy as np
import tensorflow as tf

from ml_common import (
    MODELS_DIR,
    PROCESSED_DIR,
    RESULTS_DIR,
    RANDOM_STATE,
    add_channel_axis,
    ensure_dirs,
    evaluate_predictions,
    leave_one_rpm_group_split,
    load_npz_dataset,
)


LABELS = ["Normal", "IR", "OR", "B"]


def representative_dataset(X_train: np.ndarray):
    rng = np.random.default_rng(RANDOM_STATE)
    n = min(300, len(X_train))
    chosen = rng.choice(len(X_train), size=n, replace=False)
    for idx in chosen:
        yield [X_train[idx : idx + 1].astype(np.float32)]


def run_int8_interpreter(interpreter: tf.lite.Interpreter, X_test: np.ndarray) -> np.ndarray:
    input_details = interpreter.get_input_details()[0]
    output_details = interpreter.get_output_details()[0]
    in_scale, in_zero = input_details["quantization"]
    out_scale, out_zero = output_details["quantization"]
    if in_scale == 0:
        raise ValueError("Interpreter input scale is zero.")

    input_index = input_details["index"]
    output_index = output_details["index"]
    y_prob = []

    for sample in X_test:
        quantized = np.round(sample / in_scale + in_zero)
        quantized = np.clip(quantized, -128, 127).astype(np.int8)
        interpreter.set_tensor(input_index, quantized[np.newaxis, ...])
        interpreter.invoke()
        raw = interpreter.get_tensor(output_index)[0]
        if output_details["dtype"] == np.int8:
            output = (raw.astype(np.float32) - out_zero) * out_scale
        else:
            output = raw.astype(np.float32)
        y_prob.append(output)

    return np.asarray(y_prob, dtype=np.float32)


def main() -> None:
    ensure_dirs()
    model_path = MODELS_DIR / "4class_fault_classifier.keras"
    stats_path = MODELS_DIR / "4class_norm_stats.npz"
    tflite_path = MODELS_DIR / "4class_fault_classifier_int8.tflite"

    if not model_path.exists():
        raise FileNotFoundError(f"Train the 4-class model first: {model_path}")
    if not stats_path.exists():
        raise FileNotFoundError(f"Missing normalization stats: {stats_path}")

    X, y, groups, rpm = load_npz_dataset(PROCESSED_DIR / "cwru_fft_4class.npz")
    split = leave_one_rpm_group_split(X, y, groups, rpm)
    stats = np.load(stats_path)
    mean = np.float32(stats["mean"])
    std = np.float32(stats["std"])
    X_norm = ((X - mean) / std).astype(np.float32)
    X_train = add_channel_axis(X_norm[split["train"]])
    X_test = add_channel_axis(X_norm[split["test"]])

    model = tf.keras.models.load_model(model_path)
    float_prob = model.predict(X_test, verbose=0)
    float_pred = float_prob.argmax(axis=1)
    float_metrics = evaluate_predictions(
        y[split["test"]],
        float_pred,
        LABELS,
        "4-class float model held-out RPM 1772 test before INT8 export",
        RESULTS_DIR / "4class_float_metrics_at_export.json",
    )

    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    converter.representative_dataset = lambda: representative_dataset(X_train)
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    converter.inference_input_type = tf.int8
    converter.inference_output_type = tf.int8
    tflite_model = converter.convert()
    tflite_path.write_bytes(tflite_model)
    print(f"Saved INT8 TFLite model: {tflite_path}")

    interpreter = tf.lite.Interpreter(model_path=str(tflite_path))
    interpreter.allocate_tensors()
    int8_prob = run_int8_interpreter(interpreter, X_test)
    int8_pred = int8_prob.argmax(axis=1)
    int8_metrics = evaluate_predictions(
        y[split["test"]],
        int8_pred,
        LABELS,
        "4-class TRUE INT8 TFLite held-out RPM 1772 test",
        RESULTS_DIR / "4class_int8_metrics.json",
    )

    print(
        "Float vs INT8 accuracy: "
        f"{float_metrics['accuracy']:.4f} vs {int8_metrics['accuracy']:.4f}"
    )


if __name__ == "__main__":
    main()
