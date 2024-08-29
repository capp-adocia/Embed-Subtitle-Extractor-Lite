# -*- mode: python ; coding: utf-8 -*-

a = Analysis(
    ['OCR.py'],
    pathex=[
        'C:\\Application\\Python3.8\\Lib\\site-packages\\paddleocr',
        'C:\\Application\\Python3.8\\Lib\\site-packages\\paddle\\libs'
    ],
    binaries=[('C:\\Application\\Python3.8\\Lib\\site-packages\\paddle\\libs', '.')],
    datas=[],
    hiddenimports=[],
    hookspath=['.'],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='OCR',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=True,  # Changed to True for console mode
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    icon=[],
)
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='OCR',
)
