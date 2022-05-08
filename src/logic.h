
typedef enum
{
  empty,
  human,
  ai
} Player;

typedef struct
{
  Player player;
} GridPos;


Player isWinner(GridPos grid[]);