#!/usr/bin/env python3
import yaml
import shutil
from pathlib import Path

# QoS string -> Humble integer mappings
QOS_MAP = {
    "history": {
        "system_default": 0,
        "keep_last": 1,
        "keep_all": 2,
        "unknown": 3,
    },
    "reliability": {
        "system_default": 0,
        "reliable": 1,
        "best_effort": 2,
        "unknown": 3,
    },
    "durability": {
        "system_default": 0,
        "transient_local": 1,
        "volatile": 2,
        "unknown": 3,
    },
    "liveliness": {
        "system_default": 0,
        "automatic": 1,
        "manual_by_topic": 2,
        "unknown": 3,
    },
}

def convert_qos_profile(profile):
    """Convert one Jazzy QoS profile dict to Humble inline string format."""
    lines = []
    for key, val in profile.items():
        if key in QOS_MAP and isinstance(val, str):
            val = QOS_MAP[key].get(val, val)
        if key in ("deadline", "lifespan", "liveliness_lease_duration"):
            lines.append(f"  {key}:")
            lines.append(f"    sec: {val['sec']}")
            lines.append(f"    nsec: {val['nsec']}")
        else:
            lines.append(f"  {key}: {val}")
    return "\n".join(lines)

def convert_metadata(input_path, output_path):
    with open(input_path, "r") as f:
        data = yaml.safe_load(f)

    # Remove Jazzy-only top-level fields
    data.pop("custom_data", None)
    data.pop("ros_distro", None)

    # Convert each topic
    for topic in data.get("topics_with_message_count", []):
        meta = topic["topic_metadata"]

        # Remove type_description_hash
        meta.pop("type_description_hash", None)

        # Convert offered_qos_profiles
        qos_list = meta.get("offered_qos_profiles", [])
        if isinstance(qos_list, list) and len(qos_list) > 0:
            # Keep only first profile (Humble doesn't support duplicates)
            profile = qos_list[0]
            humble_qos_str = "- " + convert_qos_profile(profile)
            meta["offered_qos_profiles"] = humble_qos_str

    with open(output_path, "w") as f:
        yaml.dump(data, f, default_flow_style=False, sort_keys=False, allow_unicode=True)

    print(f"Converted: {output_path}")

if __name__ == "__main__":
    src = Path("metadata.yaml")
    backup = Path("metadata_jazzy.yaml.bak")
    dst = Path("metadata.yaml")

    if not src.exists():
        print("metadata.yaml not found in current directory!")
        exit(1)

    # Backup original
    shutil.copy(src, backup)
    print(f"Backup created: {backup}")

    convert_metadata(src, dst)
    print("Done! You can now run: ros2 bag play .")
