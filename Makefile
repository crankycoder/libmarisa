all:
	./gradlew build

jni_stubs:
	# This is the dumbest thing ever.  Why in the hell doesn't the IDE
	# do this for me????
	javah -d app/src/main/jni -classpath ~/adt-bundle-mac/sdk/platforms/android-19/android.jar:/Users/victorng/adt-bundle-mac/sdk/extras/android/support/v7/appcompat/libs/android-support-v7-appcompat.jar:/Users/victorng/adt-bundle-mac/sdk/extras/android/support/v4/android-support-v4.jar:app/build/intermediates/classes/debug/  com.crankycoder.ndk1.AndroidNDK1SampleActivity

install:
	./gradlew installDebug

clean:
	./gradlew clean
	rm -rf app/src/main/libs
	rm -rf app/src/main/obj
	rm -rf build
