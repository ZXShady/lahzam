def generate_bin_search(start, end, indent=0):
    if start >= end:
        return ""
    mid = (start + end) // 2
    members = ",".join([f"_{j}" for j in range(0, mid)])
    indent_str = ""#"    " * indent
    code = []

    code.append(f'{indent_str}if constexpr (C == {mid}) {{ auto& [{members}] = object; return t({members}); }}')
    if start < mid:
        code.append(f'{indent_str}else if constexpr (C < {mid}) {{')
        code.append(generate_bin_search(start, mid, indent + 1))
        code.append(f'{indent_str}}}')
    if mid + 1 < end:
        code.append(f'{indent_str}else {{')
        code.append(generate_bin_search(mid + 1, end, indent + 1))
        code.append(f'{indent_str}}}')
    
    return "".join(code)


# Usage
print(generate_bin_search(1, 256))
