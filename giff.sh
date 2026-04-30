RED_BG="[48;2;59;11;0m"
RED_HL="[48;2;178;38;0m"
GREEN_HL="[48;2;25;148;0m"
GREEN_BG="[48;2;10;59;0m"
DEFAULT="[0m"

COLUMNS=$(tput cols)
TAB_LENGHT=$(echo $'\t' | wc -L)
a_stack=()
b_stack=()


compare() {
    local -a a=("${a_stack[@]}")
    local -a b=("${b_stack[@]}")
    local la=${#a[@]} lb=${#b[@]}

    #LCS table
    local -a dp
    for (( i=0; i<=la; i++ )); do
        for (( j=0; j<=lb; j++ )); do
            dp[$i,$j]=0
        done
    done

    for (( i=1; i<=la; i++ )); do
        for (( j=1; j<=lb; j++ )); do
            if [[ "${a[$((i-1))]}" == "${b[$((j-1))]}" ]]; then
                dp[$i,$j]=$(( dp[$((i-1)),$((j-1))] + 1 ))
            else
                local u=${dp[$((i-1)),$j]}
                local v=${dp[$i,$((j-1))]}
                dp[$i,$j]=$(( u > v ? u : v ))
            fi
        done
    done

    # Reconstruction du diff par backtracking
    local _a="" _b=""
    local i=$la j=$lb
    local -a ops  # "eq", "del", "ins"

    while (( i > 0 || j > 0 )); do
        if (( i > 0 && j > 0 )) && [[ "${a[$((i-1))]}" == "${b[$((j-1))]}" ]]; then
            ops=("eq ${a[$((i-1))]}" "${ops[@]}")
            (( i-- )); (( j-- ))
        elif (( j > 0 )) && { (( i == 0 )) || (( dp[$i,$((j-1))] >= dp[$((i-1)),$j] )); }; then
            ops=("ins ${b[$((j-1))]}" "${ops[@]}")
            (( j-- ))
        else
            ops=("del ${a[$((i-1))]}" "${ops[@]}")
            (( i-- ))
        fi
    done

    for op in "${ops[@]}"; do
        local kind="${op%% *}"
        local val="${op#* }"
				val=$(expand -t $((TAB_LENGHT - 1)) <<<"$val")
        case "$kind" in
            eq)  _a+="${RED_BG}${val}"; _b+="${GREEN_BG}${val}" ;;
            del) _a+="${RED_HL}${val}" ;;
            ins) _b+="${GREEN_HL}${val}" ;;
        esac
    done

		printf '%b' "${_a}${DEFAULT}"
		printf '%b' "${_b}${DEFAULT}"
}

flush() {
    if [[ ${#a_stack[@]} -gt 0 || ${#b_stack[@]} -gt 0 ]]; then
        compare
        a_stack=()
        b_stack=()
    fi
}

while IFS= read -r line; do
    first=${line:0:1}
    triple=${line:0:3}

    if [[ "$first" == "-" && "$triple" != "---" ]]; then
        # flush if reading + before
        if [[ ${#b_stack[@]} -gt 0 ]]; then
            flush
        fi
        readarray -t chunk < <(grep -oP '[[:alpha:]]+|[^[:alpha:]]' <<< "${line:1}")
        a_stack+=("-${chunk[@]}\n")

    elif [[ "$first" == "+" && "$triple" != "+++" ]]; then
        readarray -t chunk < <(grep -oP '[[:alpha:]]+|[^[:alpha:]]' <<< "${line:1}")
        b_stack+=("+${chunk[@]}\n")

    else
        flush
        printf '%s\n' "$line"
    fi
done

flush 
