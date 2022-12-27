from numpy import linspace


def print_grid(grid):
    # 0 = " ", 1 = "X", 2 = "O"
    for row in grid:
        for col in row:
            if col == 0:
                print(" ", end="") # print without newline
            elif col == 1:
                print("#", end="") # print without newline
            elif col == 2:
                print("O", end="") # print without newline
            elif col == 3:
                print("X", end="") # print without newline
            else:
                print("?", end="") # print without newline
        print()

def get_sign_of_dif(x, y):
    if x - y == 0:
        return 1
    else:
        return (y - x) // abs(y - x)

def linear_interpolation(x1, y1, x2, y2, t):
    return y1 + (y2 - y1) / (x2 - x1) * (t - x1)

def check_line(grid, x1, y1, x2, y2):
    # if the line is vertical
    if x1 == x2:
        if y1 < y2:
            for y in range(y1, y2 + 1):
                if grid[y][x1] == 1:
                    return True
        else:
            for y in range(y1, y2 - 1, -1):
                if grid[y][x1] == 1:
                    return True
    # if the line is horizontal
    elif y1 == y2:
        if x1 < x2:
            for x in range(x1, x2 + 1):
                if grid[y1][x] == 1:
                    return True
        else:
            for x in range(x1, x2 - 1, -1):
                if grid[y1][x] == 1:
                    return True
    # if the line is diagonal
    else:
        n1 = abs(x1-x2)+1
        n2 = abs(y1-y2)+1

        if (n1 < n2):
            n = n2
        else:
            n = n1

        if x1 < x2:
            for t in linspace(x1, x2, n):
                x = round(t)
                y = round(linear_interpolation(x1, y1, x2, y2, t))
                print(f"(x1, y1) = ({x1}, {y1}), (x2, y2) = ({x2}, {y2}), t = {t}, x = {x}, y = {y}")
                if grid[y][x] == 1:
                    print("wall found")
                    return True
        else:
            for t in linspace(x2, x1, n):
                x = round(t)
                y = round(linear_interpolation(x1, y1, x2, y2, t))
                print(f"(x1, y1) = ({x1}, {y1}), (x2, y2) = ({x2}, {y2}), t = {t}, x = {x}, y = {y}")
                if grid[y][x] == 1:
                    print("wall found")
                    return True
    return False

def is_behind_wall(grid, col, row, center_row, center_col):
    res = check_line(grid, center_col, center_row, col, row)
    print("-----------------")
    return res

if __name__ == "__main__":
    row_nb = 10
    col_nb = 20
    # a nxn grid
    grid = [[0 for i in range(col_nb)] for j in range(row_nb)]

    r = 1
    center_row = 5
    center_col = 10

    # right wall
    # wall_height = 3
    # for i in range(wall_height):
    #     grid[center_row - wall_height//2  + i][center_col + 1] = 1

    # left wall
    # wall_height = 3
    # for i in range(wall_height):
    #     grid[center_row - wall_height//2  + i][center_col - 1] = 1

    # bottom wall
    # wall_width = 3
    # for i in range(wall_width):
    #     grid[center_row + 1][center_col - wall_width//2 + i] = 1

    # top wall
    # wall_width = 3
    # for i in range(wall_width):
    #     grid[center_row - 1][center_col - wall_width//2 + i] = 1

    # ones on the left and right
    for i in range(col_nb):
        grid[0][i] = 1
        grid[row_nb - 1][i] = 1
    # ones on the top and bottom
    for i in range(row_nb):
        grid[i][0] = 1
        grid[i][col_nb - 1] = 1

    # a circle of radius r centered at (center_row, center_col) represented by 2
    for row in range(row_nb):
        for col in range(col_nb):
            if (row - center_row)**2 + (col - center_col)**2 <= r**2:
                if is_behind_wall(grid, col, row, center_row, center_col):
                    if grid[row][col] == 1:
                        grid[row][col] = 1
                    else:
                        grid[row][col] = 0
                    
                else:
                    grid[row][col] = 2

    # print the grid
    print_grid(grid)
