
# BAKDROID


```
            +-+-+-+-+-+-+-+-+
            |B|a|k|D|r|o|i|d|
            +-+-+-+-+-+-+-+-+
```
A hxo module that can be used to import/export application data on android without root privileges.

 
## _REQUIRES_
 1. A **hxo loaded**(v1.2.0+) apk. Details can be found [here](https://github.com/bitwaree/HXO-loader/blob/master/android/Guide-for-Android.md).

## Usage

**Setup**:

   1. Get the latest build form the [releases](https://github.com/bitwaree/bakdroid/releases).

   2. The proper `hxo` module along with the `bakdroid.ini` file is copied in the application's specific `hxo_dir`. It's mostly `"/storage/emulated/0/Android/media/<APP_ID>/modules"`.


**To backup**:

   1. Firstly, the task is needed to be specified in the `bakdroid.ini` file:
      ```ini
      task=1    ;`1` tells bakdroid to perform a backup
      ```
    
   2. Now the app/game is run. It should automatically exit by itself indicating a successful backup.

   3. Finally, the file `"/storage/emulated/0/Android/media/<APP_ID>/BAKDROID...tar.gz"` is saved in a secure location for later restoration.


**To restore a previously made backup**:

   1. Just like preforming backup, the task is needed to be specified in the `bakdroid.ini` file:
      ```ini
      task=2    ;`2` tells bakdroid to perform a restoration
      ```

   2. Copy the previously backed up file in the media folder `"/storage/emulated/0/Android/media/<APP_ID>/"`. _NOTE: make sure there is only one backup file in that location and the file must start with `BAKDROID` and end with `.tar.gz`._

   3. Finally, the app/game is run. It should automatically exit by itself indicating a successful backup restoration.



## **NOTE(s)**
-----
   - This project is made with the intention of maximizing the ability of Android apps and potentially extending them. It should not be used for any illegal means.
   - Using it on proprietary software can cause bans and loss of data.
   - The author of this project is not responsible for any damage that has been made by the use of this tool.
   - By using this tool, you are accepting the potential consequences and proceeding at your own risk.
-----

## Credits

- [inih](https://github.com/benhoyt/inih) by Ben Hoyt.
- [hxo-loader](https://github.com/bitwaree/HXO-loader) by me : )
