struct Alec {
    int x;
    int y;
};

Alec make_alec() {
    Alec out = {
        x: 1,
        y: 2
    };

    return out;
};

int main() {
    Alec a = make_alec();
    int out = a.x;

    return out;
};
