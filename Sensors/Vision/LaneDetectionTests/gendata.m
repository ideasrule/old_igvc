t = (1:100)';
x = t.*t + 3*t - 7;
xclean = x;
x = x + 1500*rand(size(xclean));
xdata = [t, x];
save 'xdata.txt' xdata -ASCII
