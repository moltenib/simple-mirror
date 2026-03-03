#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <dist-dir> [search-dir...]" >&2
    exit 1
fi

DIST_DIR="$1"
shift

if [[ ! -d "${DIST_DIR}" ]]; then
    echo "collect-win-dlls: dist directory not found: ${DIST_DIR}" >&2
    exit 1
fi

if ! command -v ldd >/dev/null 2>&1; then
    echo "collect-win-dlls: ldd not found" >&2
    exit 1
fi

declare -a search_dirs=()
for dir in "$@"; do
    if [[ -n "${dir}" && -d "${dir}" ]]; then
        search_dirs+=("${dir}")
    fi
done

if [[ "${COLLECT_WIN_DLLS_USE_PATH:-0}" == "1" ]]; then
    IFS=':' read -r -a path_dirs <<< "${PATH:-}"
    for dir in "${path_dirs[@]}"; do
        if [[ -n "${dir}" && -d "${dir}" ]]; then
            search_dirs+=("${dir}")
        fi
    done
fi

declare -A seen_search_dirs=()
declare -a normalized_search_dirs=()
for dir in "${search_dirs[@]}"; do
    abs_dir="$(cd "${dir}" 2>/dev/null && pwd || true)"
    if [[ -z "${abs_dir}" ]]; then
        continue
    fi
    if [[ -n "${seen_search_dirs[${abs_dir}]:-}" ]]; then
        continue
    fi
    seen_search_dirs["${abs_dir}"]=1
    normalized_search_dirs+=("${abs_dir}")
done

resolve_dependency() {
    local dep_name="$1"
    local dep_path="$2"

    case "${dep_name}" in
    api-ms-win-* | ext-ms-win-*)
        return 1
        ;;
    esac

    if [[ -n "${dep_path}" && -f "${dep_path}" ]]; then
        case "${dep_path}" in
        /c/Windows/* | /c/WINDOWS/* | /cygdrive/c/Windows/* | /cygdrive/c/WINDOWS/*)
            return 1
            ;;
        esac
        printf '%s\n' "${dep_path}"
        return 0
    fi

    for dir in "${normalized_search_dirs[@]}"; do
        if [[ -f "${dir}/${dep_name}" ]]; then
            printf '%s\n' "${dir}/${dep_name}"
            return 0
        fi
        local case_insensitive
        case_insensitive="$(find "${dir}" -maxdepth 1 -type f -iname "${dep_name}" | head -n1 || true)"
        if [[ -n "${case_insensitive}" ]]; then
            printf '%s\n' "${case_insensitive}"
            return 0
        fi
    done

    return 1
}

declare -a queue=()
while IFS= read -r -d '' file; do
    queue+=("${file}")
done < <(find "${DIST_DIR}" -type f \( -iname '*.exe' -o -iname '*.dll' \) -print0)

echo "collect-win-dlls: scanning ${#queue[@]} module(s) in ${DIST_DIR}"

declare -A seen_modules=()
queue_index=0
while ((queue_index < ${#queue[@]})); do
    module="${queue[queue_index]}"
    ((queue_index += 1))

    module_key="$(cd "$(dirname "${module}")" && pwd)/$(basename "${module}")"
    if [[ -n "${seen_modules[${module_key}]:-}" ]]; then
        continue
    fi
    seen_modules["${module_key}"]=1

    ldd_output="$(ldd "${module}" 2>/dev/null || true)"
    if [[ -z "${ldd_output}" ]]; then
        continue
    fi

    while IFS= read -r line; do
        line="${line//$'\r'/}"
        [[ "${line}" == *"=>"* ]] || continue

        dep_name="$(awk '{print $1}' <<<"${line}")"
        dep_path="$(awk -F'=> ' '{print $2}' <<<"${line}" | awk '{print $1}')"
        if [[ "${dep_path}" == "not" ]]; then
            dep_path=""
        fi

        resolved_path="$(resolve_dependency "${dep_name}" "${dep_path}" || true)"
        if [[ -z "${resolved_path}" || ! -f "${resolved_path}" ]]; then
            continue
        fi

        dest_path="${DIST_DIR}/$(basename "${resolved_path}")"
        if [[ ! -f "${dest_path}" ]]; then
            cp -f "${resolved_path}" "${dest_path}"
        fi
        queue+=("${dest_path}")
    done <<< "${ldd_output}"

    if ((queue_index % 50 == 0)); then
        echo "collect-win-dlls: processed ${queue_index}/${#queue[@]}"
    fi
done

echo "collect-win-dlls: dependency copy complete for ${DIST_DIR}"
