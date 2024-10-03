void setup();
void repeat();

int main() {
    PORTB = 0;
    PINB = 0;

    setup();

    while (true) {
        repeat();
    }
}
