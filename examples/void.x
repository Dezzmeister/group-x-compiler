type ReturnsNothing = [int] -> void.
type AcceptsNothing = [void] -> int.
type AcceptsNothing2 = [] -> int.
type TuplesInRet = [] -> [int, float].
type NestedBrackets = [[int, int]] -> void.

void returns_nothing(int a) {
    print(a).
}.

int accepts_nothing() {
    return 5.
}.
