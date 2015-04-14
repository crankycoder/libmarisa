all:
	./gradlew build

lib:
	cd jni && make

install:
	./gradlew installDebug

clean:
	./gradlew clean
	rm -rf lib
