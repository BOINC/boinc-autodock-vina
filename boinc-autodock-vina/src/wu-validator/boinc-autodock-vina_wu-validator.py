import hashlib
import sys
import zipfile

def calculate_wu_hash(wu):
    with zipfile.ZipFile(wu, 'r') as zip_ref:
        if len(zip_ref.filelist) != 1:
            sys.exit(1)
        pdbqt = zip_ref.filelist[0]
        if not pdbqt.filename.endswith('.pdbqt'):
            sys.exit(1)
        with zip_ref.open(pdbqt) as f:
            return hashlib.sha256(f.read()).hexdigest()

if len(sys.argv) != 3:
    print("Usage: boinc-autodock-vina_wu-validator.py <workunit1> <workunit2>")
    sys.exit(1)

wu1 = sys.argv[1]
wu2 = sys.argv[2]

hash1 = calculate_wu_hash(wu1)
hash2 = calculate_wu_hash(wu2)

if hash1 == hash2 and hash1 is not None and hash2 is not None:
    sys.exit(0)

sys.exit(1)
