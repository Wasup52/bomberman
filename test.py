import math

from numpy import linspace


def linear_interpolation(x1, y1, x2, y2, t):
    return y1 + (y2 - y1) / (x2 - x1) * (t - x1)

def draw_line(grid, x1, y1, x2, y2):
    i = 1

    # if the line is vertical
    if x1 == x2:
        if y1 < y2:
            # don't draw (x1, y1)
            for y in range(y1 + 1, y2 + 1):
                grid[y][x1] = 2
                i += 1
        else:
            # don't draw (x1, y1)
            for y in range(y1 - 1, y2 - 1, -1):
                grid[y][x1] = 2
                i += 1
    # if the line is horizontal
    elif y1 == y2:
        if x1 < x2:
            for x in range(x1, x2 + 1):
                # don't draw (x1, y1)
                if x != x1:
                    grid[y1][x] = 2
                    i += 1
        else:
            for x in range(x1, x2 - 1, -1):
                # don't draw (x1, y1)
                if x != x1:
                    grid[y1][x] = 2
                    i += 1
    # if the line is diagonal
    else:
        nb1 = abs(x1-x2)+1
        nb2 = abs(y1-y2)+1

        if (nb1 < nb2):
            nb = nb2
        else:
            nb = nb1

        if x1 < x2:
            for t in linspace(x1, x2, nb):
                x = round(t)
                if x == x1:
                    continue
                else:
                    y = round(linear_interpolation(x1, y1, x2, y2, t))
                    print(f"x1 < x2, t = {t}, x = {x}, y = {y}")
                    grid[y][x] = 2
                    i += 1
        else:
            for t in linspace(x2, x1, nb):
                x = round(t)
                if x == x1:
                    continue
                else:
                    y = round(linear_interpolation(x1, y1, x2, y2, t))
                    print(f"x1 >= x2, t = {t}, x = {x}, y = {y}")
                    grid[y][x] = 2
                    i += 1


def print_grid(grid):
    # 0 = " ", 1 = "X", 2 = "O"
    for row in grid:
        for col in row:
            if col == 0:
                print(" ", end="") # print without newline
            else:
                print(col, end="") # print without newline
        print()

row_nb = 10
col_nb = 20

grid = [[0 for i in range(col_nb)] for j in range(row_nb)]

# ones on the left and right
for i in range(col_nb):
    grid[0][i] = 1
    grid[row_nb - 1][i] = 1
# ones on the top and bottom
for i in range(row_nb):
    grid[i][0] = 1
    grid[i][col_nb - 1] = 1


center_row = 5
center_col = 10

# draw a line between two points
# draw_line(grid, center_col, center_row, 3, 5)

# draw a circle around a point
r = 4

grid[center_row][center_col] = 4 # draw the center

prev_grid = []
for row in grid:
    prev_grid.append(row.copy())

for i in range(360):

    x = round(r * math.cos(math.radians(i)))
    y = round(r * math.sin(math.radians(i)))

    draw_line(grid, center_col, center_row, center_col + x, center_row + y)
    grid[center_row + y][center_col + x] = 4 # draw the outer circle

    # condition = grid != prev_grid

    # if condition:
    #     prev_grid = []
    #     for row in grid:
    #         prev_grid.append(row.copy())

    #     print_grid(grid)
        
    #     grid = [[0 for i in range(col_nb)] for j in range(row_nb)]

    #     # ones on the left and right
    #     for i in range(col_nb):
    #         grid[0][i] = 1
    #         grid[row_nb - 1][i] = 1
    #     # ones on the top and bottom
    #     for i in range(row_nb):
    #         grid[i][0] = 1
    #         grid[i][col_nb - 1] = 1

# draw_line(grid, center_col, center_row, 8, 2)
print_grid(grid)