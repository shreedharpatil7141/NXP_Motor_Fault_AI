from __future__ import annotations

import tensorflow as tf

from ml_common import (
    MODELS_DIR,
    PROCESSED_DIR,
    RESULTS_DIR,
    add_channel_axis,
    class_weight_dict,
    ensure_dirs,
    evaluate_predictions,
    leave_one_rpm_group_split,
    load_npz_dataset,
    normalize_from_train,
    print_distribution,
    save_norm_stats,
)


LABELS = ["Normal", "IR", "OR", "B"]


def build_model(n_bins: int) -> tf.keras.Model:
    model = tf.keras.Sequential(
        [
            tf.keras.layers.Input(shape=(n_bins, 1)),
            tf.keras.layers.Conv1D(16, kernel_size=7, strides=2, padding="same"),
            tf.keras.layers.BatchNormalization(),
            tf.keras.layers.ReLU(),
            tf.keras.layers.MaxPooling1D(pool_size=2),
            tf.keras.layers.Conv1D(32, kernel_size=5, strides=2, padding="same"),
            tf.keras.layers.BatchNormalization(),
            tf.keras.layers.ReLU(),
            tf.keras.layers.MaxPooling1D(pool_size=2),
            tf.keras.layers.Conv1D(64, kernel_size=3, padding="same"),
            tf.keras.layers.BatchNormalization(),
            tf.keras.layers.ReLU(),
            tf.keras.layers.GlobalAveragePooling1D(),
            tf.keras.layers.Dense(64, activation="relu"),
            tf.keras.layers.Dropout(0.3),
            tf.keras.layers.Dense(4, activation="softmax"),
        ]
    )
    model.compile(
        optimizer=tf.keras.optimizers.Adam(),
        loss="sparse_categorical_crossentropy",
        metrics=["accuracy"],
    )
    return model


def main() -> None:
    ensure_dirs()
    X, y, groups, rpm = load_npz_dataset(PROCESSED_DIR / "cwru_fft_4class.npz")
    split = leave_one_rpm_group_split(X, y, groups, rpm)
    X_norm, mean, std = normalize_from_train(X, split["train"])

    print(f"Dataset X shape: {X.shape}")
    for name, idx in split.items():
        unique_groups = len(set(groups[idx].tolist()))
        rpms = sorted(set(rpm[idx].tolist()))
        print_distribution(f"{name} split | groups={unique_groups} | rpm={rpms}", y[idx], LABELS)

    model = build_model(X.shape[1])
    callbacks = [
        tf.keras.callbacks.EarlyStopping(
            monitor="val_loss", patience=10, restore_best_weights=True
        ),
        tf.keras.callbacks.ReduceLROnPlateau(monitor="val_loss", patience=5, factor=0.5),
    ]
    weights = class_weight_dict(y[split["train"]])
    print(f"Class weights: {weights}")

    model.fit(
        add_channel_axis(X_norm[split["train"]]),
        y[split["train"]],
        validation_data=(add_channel_axis(X_norm[split["val"]]), y[split["val"]]),
        epochs=100,
        batch_size=64,
        callbacks=callbacks,
        class_weight=weights,
        verbose=2,
    )

    test_prob = model.predict(add_channel_axis(X_norm[split["test"]]), verbose=0)
    y_pred = test_prob.argmax(axis=1)
    evaluate_predictions(
        y[split["test"]],
        y_pred,
        LABELS,
        "4-class float model held-out RPM 1772 test",
        RESULTS_DIR / "4class_float_metrics.json",
    )

    model_path = MODELS_DIR / "4class_fault_classifier.keras"
    stats_path = MODELS_DIR / "4class_norm_stats.npz"
    model.save(model_path)
    save_norm_stats(stats_path, mean, std)
    print(f"Saved float model: {model_path}")


if __name__ == "__main__":
    main()
