# SA2B Free Camera
Port of the free camera mode from Sonic Adventure DX to Sonic Adventure 2 Battle (Steam version.)

Enable it through the new dedicated option in the pause menu or press L+R. It is compatible with multiplayer.

## How to install

You need the [SA2 Mod Loader](https://github.com/X-Hax/sa2-mod-loader) to install this mod.

Download the latest version from the release page, then extract the root folder in the archive into the "mods" folder. Make sure you do not extract the individual files in the mods folder, they should be in a subfolder.

## How to configure

You can configure the mod in the Mod Manager by clicking on the mod, then on the "configure" button.

Alternatively, you can create a "config.ini" file in the mod's folder and fill in the configuration manually. Here is the list of parameters with their default values:

```
Mode=True ; if the camera option should be added to the pause menu (it replaces the leaderboard option)
AnalogThreshold=64 ; the sensitivity when using the right analog or LR triggers
AnalogMaxSpeed=256 ; the maximum speed at which the camera can rotate when using the right analog or LR triggers
MinDist=16 ; the closest distance that the camera can be from the player
MaxDist=50 ; the farthest distance that the camera can be from the player
Height=10,5 ; the height of the camera (from Sonic's perspective, automatically adjusted for other characters)
InvertAxes=False ; inverts the right-analog axes
```

## Credits

Original code from Sonic Adventure DX: Director's Cut © SEGA/SONIC TEAM.

Ported to Sonic Adventure 2: Battle © SEGA/SONIC TEAM using elements reverse-engineered from the game.

Translated to all of the languages from the original game with help from kawaiikaorichan (Spanish) and Nim (German)
