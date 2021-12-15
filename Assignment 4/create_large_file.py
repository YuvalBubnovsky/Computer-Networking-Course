with open("1gb.txt", "w") as out:
    out.seek((1024 * 1024) - 1)
    out.write('\0')