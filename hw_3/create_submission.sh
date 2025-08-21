ID=ex3_322868852_340849710

make clean

mkdir $ID

cp -r Algorithm $ID/Algorithm
cp -r common $ID/common
cp -r GameManager $ID/GameManager
cp -r Simulator $ID/Simulator
cp -r UserCommon $ID/UserCommon

rm $ID/Algorithm/CMakeLists.txt
rm $ID/GameManager/CMakeLists.txt
rm $ID/Simulator/CMakeLists.txt

cp Makefile $ID/Makefile
cp README.md $ID/README.md
cp students.txt $ID/students.txt

zip -r $ID.zip $ID

rm -r $ID
