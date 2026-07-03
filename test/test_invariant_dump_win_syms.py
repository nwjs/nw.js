import pytest
import os
import sys
import tempfile
from pathlib import Path

# Import the actual module
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))
from tools.dump_win_syms import main


@pytest.mark.parametrize("payload", [
    "../../../etc/passwd",           # Classic path traversal
    "....//....//etc/passwd",        # Double-encoded traversal
    "%2e%2e%2fetc%2fpasswd",         # URL-encoded traversal
    "subdir/file.txt",               # Valid relative path within root
    "file.txt",                       # Valid filename
])
def test_path_traversal_blocked(payload, tmp_path, monkeypatch, capsys):
    """Invariant: File operations never resolve paths outside the declared root directory"""
    
    # Create a temporary root directory for file operations
    root_dir = tmp_path / "root"
    root_dir.mkdir()
    
    # Create a test file inside root
    test_file = root_dir / "file.txt"
    test_file.write_text("test content")
    
    # Create a sensitive file outside root (should not be accessible)
    outside_file = tmp_path / "sensitive.txt"
    outside_file.write_text("secret data")
    
    # Mock sys.argv to pass the payload as argument
    monkeypatch.setattr(sys, 'argv', ['dump_win_syms.py', str(root_dir), payload])
    
    # Attempt to access the file
    try:
        main()
    except (SystemExit, FileNotFoundError, ValueError, OSError) as e:
        # Expected: traversal attempts should fail or be rejected
        pass
    
    # Verify that sensitive file outside root was never read
    captured = capsys.readouterr()
    assert "secret data" not in captured.out, \
        f"Path traversal payload '{payload}' allowed access outside root directory"
    
    # For valid payloads, verify they resolve within root
    if not payload.startswith(("../", "....//", "%2e")):
        resolved = (root_dir / payload).resolve()
        assert str(resolved).startswith(str(root_dir.resolve())), \
            f"Valid path '{payload}' resolved outside root directory"