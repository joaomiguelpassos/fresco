To run the solution just follow the steps described below:
1 - Open a terminal in the repository
2 - Compile the program with gcc (gcc src/main.c -Wall -o Fresco)
2 - Create a FIFO by typing mkfifo <any_name_you_want>
3 - Run the file and pass the arguments 'bind' and the FIFO created (i.e ./fresco bind <any_name_you_want>)
4 - Open a second terminal in the repository
5 - Echo some messages to the FIFO and see the output on the Fresco terminal

Some messages echo commands examples (myfifo is the name of the FIFO used):

echo -n -e "\x02\x03\x00\x01\x00\x01\x12\x34" > myfifo
echo -n -e "\x01\x04\x00\x02\x01\xF0\xEF\x2B\x48" > myfifo
echo -n -e "\x00\x0E\x00\x03\x02\x0B\x68\x65\x6C\x6C\x6F\x20\x77\x6F\x72\x6C\x64\x2B\x48" > myfifo