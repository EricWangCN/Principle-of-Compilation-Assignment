//111
/*222*/
void main() {
    int sum = 0;
    for(int i = 0; i <= 10; i++) {
        sum += i << 2;
        sum += i >> 2;
        sum += 2;
        sum *= 2;
        sum /= 2;
        sum -= 2;
     if (sum == 63 && i < 5 || sum == 128)
          break;
     i!=3;
    }

    return 0;
}