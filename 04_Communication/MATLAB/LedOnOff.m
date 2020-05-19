s = serial('COM11','Baudrate', 9600);
fopen(s);
for i = 1:5
    fprintf('Turning LED ON\n');
    fprintf(s, 'ON');
    pause(2);
    fprintf('Turning LED OFF\n');
    fprintf(s, 'OFF');
    pause(2);
end
fclose(s);