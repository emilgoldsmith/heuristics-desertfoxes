#include <algorithm>

using namespace std;

const int MAX_STONES = 1010; // It's actually 1000
const int MAX_CUR_MAX = 50; // It's actually 45
const int MAX_RESETS = 6; // It's actually 4
const int MAX_FLAG_VALUES = 4; // It's actually 2

// First MAX_RESETS is resets left for next player, second one is for previous player
// the value is 0 if this is a losing position, if it is not zero it is the amount of stones to take
int dp[MAX_STONES][MAX_CUR_MAX][MAX_RESETS][MAX_RESETS][MAX_FLAG_VALUES];
// A boolean that tells us whether or not it is optimal to use reset here in combination with
// amount of stones told to take from dp
bool shouldWeReset[MAX_STONES][MAX_CUR_MAX][MAX_RESETS][MAX_RESETS][MAX_FLAG_VALUES];

void initialize() {
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

            int *ptr = &dp[stonesLeft][curMax][nextResets][prevResets][currentlyReset];
            bool *resetPtr = &shouldWeReset[stonesLeft][curMax][nextResets][prevResets][currentlyReset];

            // Initialize the dp with losing and not resetting until we find a winning move
            *ptr = 0;
            *resetPtr = false;

            // We start from the top just because of a decision that we'd rather take maximum amount of winning stones, as that'll finish the game faster, no reason not to
            for (int stonesToTake = allowedStonesToTake; stonesToTake > 0; stonesToTake--) {
              int newStonesLeft = stonesLeft - stonesToTake;
              int newCurMax = max(curMax, stonesToTake);

              int dontUseReset = dp[newStonesLeft][newCurMax][prevResets][nextResets][0];
              if (dontUseReset == 0) {
                // This means we put them in a losing position
                *ptr = max(*ptr, stonesToTake);
                *resetPtr = false;
                // We can break as we found a winning move
                break;
              }

              if (nextResets > 0) {
                // This means we have the possibility of using a reset ourselves
                int useReset = dp[newStonesLeft][newCurMax][prevResets][nextResets - 1][1];
                if (useReset == 0) {
                  // This means we put them in a losing position
                  *ptr = max(*ptr, stonesToTake);
                  *resetPtr = true;
                  // We break for same reason as above
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
}

int getMove(int curStones, int curMax, int nextResets, int prevResets, int currentlyReset, bool &shouldIReset) {
  // shouldIReset is passed by reference to provide the "second return value"
  shouldIReset = shouldWeReset[curStones][curMax][nextResets][prevResets][currentlyReset];
  // Return amount of stones to take (or 0 if losing position)
  return dp[curStones][curMax][nextResets][prevResets][currentlyReset];
}
