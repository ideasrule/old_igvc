d = importdata('outdata.txt');
dat = d.data;
hold on
plot(t, x, 'k')
plot(t, dat(:,2), 'b')
plot(t, dat(:,3), 'g')
plot(t, dat(:,4), 'r')