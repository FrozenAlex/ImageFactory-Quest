Param(
    [Parameter(Mandatory=$false)]
    [Switch] $clean,

    [Parameter(Mandatory=$false)]
    [Switch] $help,
    [Parameter(Mandatory=$false)]
    [Switch] $release
)

if ($help -eq $true) {
    Write-Output "`"Build`" - Copiles your mod into a `".so`" or a `".a`" library"
    Write-Output "`n-- Arguments --`n"

    Write-Output "-Clean `t`t Deletes the `"build`" folder, so that the entire library is rebuilt"

    exit
}

# if user specified clean, remove all build files
if ($clean.IsPresent) {
    if (Test-Path -Path "build") {
        remove-item build -R -Force 
    }
}

$buildType = "Debug"
if ($release.IsPresent) {
    $buildType = "RelWithDebInfo"
    Write-Output "Building release"
} else {
    Write-Output "Building debug"
}

if (($clean.IsPresent) -or (-not (Test-Path -Path "build"))) {
    new-item -Path build -ItemType Directory
} 


& cmake -G "Ninja" -DCMAKE_BUILD_TYPE="$buildType" -B build 
& cmake --build ./build
exit $LASTEXITCODE