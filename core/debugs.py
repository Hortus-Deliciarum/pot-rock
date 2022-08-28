from colorama import Fore


def debug(deb, *txt):
    """debug printing"""
    if deb:
        #print(Fore.CYAN + txt + Fore.WHITE)
        print(Fore.CYAN, end='')

        for t in txt:
            print(t, end=' ')

        print(Fore.WHITE)

