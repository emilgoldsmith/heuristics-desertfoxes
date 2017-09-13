#include <iostream>
#include <algorithm>

using namespace std;

const int MAX_STONES = 1010; // It's actually 1000
const int MAX_CUR_MAX = 50; // It's actually 45
const int MAX_RESETS = 6; // It's actually 4
const int MAX_FLAG_VALUES = 4; // It's actually 2

// First MAX_RESETS is resets left for next player, second one is for previous player
// the value is 0 if this is a losing position, if it is not zero it is the amount of stones to take
int dp[MAX_STONES][MAX_CUR_MAX][MAX_RESETS][MAX_RESETS][MAX_FLAG_VALUES];

int main() {
  // Initialize the base case
  // No matter the other conditions if there are no stones left the next player to play loses
  for (int curMax = 0; curMax < MAX_CUR_MAX; curMax++) {
    for (int nextResets = 0; nextResets < MAX_RESETS; nextResets++) {
      for (int prevResets = 0; prevResets < MAX_RESETS; prevResets++) {
        for (int currentlyReset = 0; currentlyReset < 2; currentlyReset++) {
          dp[0][curMax][nextResets][prevResets][currentlyReset] = 0;
        }
      }
    }
  }

  for (int stonesLeft = 1; stonesLeft < MAX_STONES; stonesLeft++) {
    for (int curMax = 0; curMax < MAX_CUR_MAX; curMax++) {
      for (int nextResets = 0; nextResets < MAX_RESETS; nextResets++) {
        for (int prevResets = 0; prevResets < MAX_RESETS; prevResets++) {
          for (int currentlyReset = 0; currentlyReset < 2; currentlyReset++) {
            int allowedStonesToTake = max(3, curMax + 1);
            if (currentlyReset) {
              allowedStonesToTake = 3;
            }
            allowedStonesToTake = min(allowedStonesToTake, stonesLeft);

            // Initialize the dp with losing until we find a winning move
            dp[stonesLeft][curMax][nextResets][prevResets][currentlyReset] = 0;
            int *ptr = &dp[stonesLeft][curMax][nextResets][prevResets][currentlyReset];

            for (int stonesToTake = 1; stonesToTake <= allowedStonesToTake; stonesToTake++) {
              int newStonesLeft = stonesLeft - stonesToTake;
              int newCurMax = max(curMax, stonesToTake);

              // First case is we don't use a reset ourselves
              *ptr = max(*ptr, dp[newStonesLeft][newCurMax][prevResets][nextResets][0]);

              if (nextResets > 0) {
                // This means we have the possibility of using a reset ourselves
                *ptr = max(*ptr, dp[newStonesLeft][newCurMax][prevResets][nextResets - 1][1]);
              }
            }
          }
        }
      }
    }
  }

}
