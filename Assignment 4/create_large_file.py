with open("100mb.txt", "w") as out:
    out.seek((1024 * 1024 * 100) - 1)
    out.write('\0')