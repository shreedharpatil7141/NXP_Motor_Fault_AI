from __future__ import annotations

import re
from pathlib import Path

import numpy as np
import pandas as pd


ROOT = Path(__file__).resolve().parents[1]
RAW_DIR = ROOT / "dataset" / "raw" / "CWRU_Bearing_NumPy"
PROCESSED_DIR = ROOT / "dataset" / "processed"
WINDOW = 1024
STRIDE = 512


def parse_rpm(path: Path) -> int:
    match = re.match(r"^(\d{4})_", path.name)
    if not match:
        raise ValueError(f"Could not parse RPM from filename: {path.name}")
    return int(match.group(1))


def fault_label(path: Path) -> tuple[int, int, str]:
    name = path.name
    if "Normal" in name:
        return 0, 0, "Normal"
    if "_IR" in name:
        return 1, 1, "IR"
    if "_OR@" in name:
        return 1, 2, "OR"
    if "_B" in name:
        return 1, 3, "B"
    raise ValueError(f"Could not infer fault type from filename: {name}")


def selected_files() -> list[Path]:
    if not RAW_DIR.exists():
        raise FileNotFoundError(
            f"Dataset not found at {RAW_DIR}. Clone CWRU_Bearing_NumPy into dataset/raw first."
        )

    files: list[Path] = []
    for path in sorted(RAW_DIR.rglob("*.npz")):
        name = path.name
        if name.endswith("_DE12.npz") or name.endswith("_Normal.npz"):
            files.append(path)
    if not files:
        raise FileNotFoundError(f"No *_DE12.npz or *_Normal.npz files found under {RAW_DIR}.")
    return files


def load_de_channel(path: Path) -> np.ndarray:
    data = np.load(path)
    if "DE" not in data:
        raise KeyError(f"{path} does not contain a DE channel. Keys: {list(data.keys())}")
    return np.asarray(data["DE"], dtype=np.float32).reshape(-1)


def fft_windows(signal: np.ndarray) -> np.ndarray:
    if len(signal) < WINDOW:
        return np.empty((0, WINDOW // 2 + 1), dtype=np.float32)

    starts = range(0, len(signal) - WINDOW + 1, STRIDE)
    spectra = []
    for start in starts:
        window = signal[start : start + WINDOW]
        magnitude = np.abs(np.fft.rfft(window))
        spectra.append(np.log1p(magnitude).astype(np.float32))
    return np.asarray(spectra, dtype=np.float32)


def main() -> None:
    PROCESSED_DIR.mkdir(parents=True, exist_ok=True)

    X_parts: list[np.ndarray] = []
    y_binary_parts: list[np.ndarray] = []
    y_4class_parts: list[np.ndarray] = []
    group_parts: list[np.ndarray] = []
    rpm_parts: list[np.ndarray] = []
    rows: list[dict[str, object]] = []

    for group_id, path in enumerate(selected_files()):
        rpm = parse_rpm(path)
        y_binary, y_4class, label = fault_label(path)
        signal = load_de_channel(path)
        spectra = fft_windows(signal)
        n = len(spectra)
        if n == 0:
            print(f"Skipping {path.name}: signal shorter than {WINDOW} samples")
            continue

        X_parts.append(spectra)
        y_binary_parts.append(np.full(n, y_binary, dtype=np.int64))
        y_4class_parts.append(np.full(n, y_4class, dtype=np.int64))
        group_parts.append(np.full(n, group_id, dtype=np.int64))
        rpm_parts.append(np.full(n, rpm, dtype=np.int64))
        rows.append(
            {
                "group": group_id,
                "file": str(path.relative_to(ROOT)),
                "rpm": rpm,
                "label": label,
                "windows": n,
            }
        )

    if not X_parts:
        raise RuntimeError("No windows were generated.")

    X = np.concatenate(X_parts, axis=0)
    y_binary = np.concatenate(y_binary_parts)
    y_4class = np.concatenate(y_4class_parts)
    groups = np.concatenate(group_parts)
    rpm = np.concatenate(rpm_parts)

    binary_path = PROCESSED_DIR / "cwru_fft_binary.npz"
    four_path = PROCESSED_DIR / "cwru_fft_4class.npz"
    np.savez_compressed(binary_path, X=X, y=y_binary, groups=groups, rpm=rpm)
    np.savez_compressed(four_path, X=X, y=y_4class, groups=groups, rpm=rpm)

    manifest = pd.DataFrame(rows)
    manifest_path = PROCESSED_DIR / "manifest.csv"
    manifest.to_csv(manifest_path, index=False)

    print(f"Window={WINDOW}, stride={STRIDE}, FFT bins={X.shape[1]}")
    print(f"Files processed: {len(rows)}")
    print(f"Total windows: {len(X)}")
    print("RPM balance:")
    print(manifest.groupby("rpm")["windows"].sum().to_string())
    print("Binary class balance:")
    print(pd.Series(y_binary).map({0: "Normal", 1: "Fault"}).value_counts().to_string())
    print("4-class balance:")
    print(pd.Series(y_4class).map({0: "Normal", 1: "IR", 2: "OR", 3: "B"}).value_counts().to_string())
    print(f"Saved: {binary_path}")
    print(f"Saved: {four_path}")
    print(f"Saved: {manifest_path}")


if __name__ == "__main__":
    main()
