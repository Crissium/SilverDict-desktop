import os
import subprocess
import sys


def get_sources() -> list[str]:
    with open('CMakeLists.txt', 'r') as f:
        cmakelists = f.read()

    start = cmakelists.find('MANUAL_FINALIZATION\n') + len('MANUAL_FINALIZATION\n')
    end = cmakelists.find('\n\t${QM_FILES}', start)
    return [
        source.strip()
        for source in cmakelists[start:end].split()
    ]


def lupdate(locale: str) -> None:
    sources = get_sources()
    subprocess.run(
        ['lupdate', '-noobsolete', *sources, '-ts', os.path.join('data', 'translations', f'SilverDict_{locale}.ts')])


if __name__ == '__main__':
    subcommand = sys.argv[1]
    match subcommand:
        case 'lupdate':
            locale = sys.argv[2]
            lupdate(locale)
        case _:
            raise ValueError(f'Unknown subcommand: {subcommand}')
