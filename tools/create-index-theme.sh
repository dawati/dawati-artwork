#! /bin/bash

if [ "x$1" = "x" ]
then
  echo "Usage: $0 [icon theme directory]"
  exit 1
fi

THEME_NAME=Dawati
THEME_COMMENT="$THEME_NAME Icon Theme"

OUTPUT=`echo "$1" | sed -e 's,/$,,'`

cd "$OUTPUT"

echo -n "Creating index.theme in ${OUTPUT}/... "

echo -e "[Icon Theme]\nName=$THEME_NAME\nComment=$THEME_COMMENT\n" > index.theme
echo "Inherits=gnome" >> index.theme
echo -n "Directories=" >> index.theme

DIRS=`find * -type d | grep -v git | grep -v scalable | grep "/"`


for foo in $DIRS
do
	echo -n "$foo," >> index.theme
done

for foo in $DIRS
do
	size=`echo $foo | cut -b -5`
	if [ "$size" = 48x48 ] ; then
		echo -en "\n\n[$foo]\nSize=`echo $size | sed 's/\x.*//'`\nContext=`basename $foo`\nType=Scalable" >> index.theme
	else
		echo -en "\n\n[$foo]\nSize=`echo $size | sed 's/\x.*//'`\nContext=`basename $foo`\nType=Fixed" >> index.theme
	fi
done

echo "Done"

cd - 1>/dev/null 2>/dev/null
