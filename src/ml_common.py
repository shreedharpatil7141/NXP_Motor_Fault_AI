from __future__ import annotations

import json
from pathlib import Path
from typing import Iterable

import numpy as np
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
from sklearn.model_selection import GroupShuffleSplit
from sklearn.utils.class_weight import compute_class_weight


ROOT = Path(__file__).resolve().parents[1]
PROCESSED_DIR = ROOT / "dataset" / "processed"
MODELS_DIR = ROOT / "models"
RESULTS_DIR = ROOT / "results"
HELD_OUT_RPM = 1772
RANDOM_STATE = 42


def ensure_dirs() -> None:
    MODELS_DIR.mkdir(parents=True, exist_ok=True)
    RESULTS_DIR.mkdir(parents=True, exist_ok=True)


def load_npz_dataset(path: Path) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    data = np.load(path)
    return data["X"].astype(np.float32), data["y"], data["groups"], data["rpm"]


def leave_one_rpm_group_split(
    X: np.ndarray,
    y: np.ndarray,
    groups: np.ndarray,
    rpm: np.ndarray,
    held_out_rpm: int = HELD_OUT_RPM,
    val_size: float = 0.15,
) -> dict[str, np.ndarray]:
    train_val_mask = rpm != held_out_rpm
    test_mask = rpm == held_out_rpm

    if not np.any(test_mask):
        raise ValueError(f"No samples found for held-out RPM {held_out_rpm}.")
    if not np.any(train_val_mask):
        raise ValueError("No samples remain for training after hold-out split.")

    train_val_idx = np.flatnonzero(train_val_mask)
    splitter = GroupShuffleSplit(
        n_splits=1,
        test_size=val_size,
        random_state=RANDOM_STATE,
    )
    train_rel, val_rel = next(
        splitter.split(X[train_val_idx], y[train_val_idx], groups[train_val_idx])
    )

    return {
        "train": train_val_idx[train_rel],
        "val": train_val_idx[val_rel],
        "test": np.flatnonzero(test_mask),
    }


def normalize_from_train(
    X: np.ndarray, train_idx: np.ndarray
) -> tuple[np.ndarray, np.float32, np.float32]:
    mean = np.float32(X[train_idx].mean())
    std = np.float32(X[train_idx].std())
    if std <= 0:
        raise ValueError("Training standard deviation is zero; cannot normalize.")
    return ((X - mean) / std).astype(np.float32), mean, std


def class_weight_dict(y_train: np.ndarray) -> dict[int, float]:
    classes = np.unique(y_train)
    weights = compute_class_weight(class_weight="balanced", classes=classes, y=y_train)
    return {int(cls): float(weight) for cls, weight in zip(classes, weights)}


def add_channel_axis(X: np.ndarray) -> np.ndarray:
    return X[..., np.newaxis].astype(np.float32)


def print_distribution(name: str, y: np.ndarray, labels: Iterable[str]) -> None:
    labels = list(labels)
    counts = np.bincount(y.astype(int), minlength=len(labels))
    summary = ", ".join(f"{label}={count}" for label, count in zip(labels, counts))
    print(f"{name}: n={len(y)} | {summary}")


def evaluate_predictions(
    y_true: np.ndarray,
    y_pred: np.ndarray,
    labels: list[str],
    title: str,
    report_path: Path | None = None,
) -> dict[str, object]:
    acc = float(accuracy_score(y_true, y_pred))
    cm = confusion_matrix(y_true, y_pred, labels=list(range(len(labels))))
    report = classification_report(
        y_true,
        y_pred,
        labels=list(range(len(labels))),
        target_names=labels,
        digits=4,
        zero_division=0,
    )

    print(f"\n{title}")
    print(f"Accuracy: {acc:.4f}")
    print("Confusion matrix:")
    print(cm)
    print("Classification report:")
    print(report)

    result = {
        "title": title,
        "accuracy": acc,
        "confusion_matrix": cm.tolist(),
        "classification_report": report,
    }
    if report_path is not None:
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(json.dumps(result, indent=2), encoding="utf-8")
    return result


def save_norm_stats(path: Path, mean: np.float32, std: np.float32) -> None:
    np.savez_compressed(path, mean=mean, std=std)
    print(f"Saved normalization stats: {path}")
