# Addnet
A type of neural network that only adds and subtracts using a bin system. Most of the code is used for the ncurses interface. The math is pretty straightfoward.

Here's the essence:

	void forward(real x)
	{
		
		f[0] = x;
		h[0] = (int)floor(x*H);

		for (int i = 1; i < W; i++)
		{
			f[i] = f[i-1]+w[i-1][h[i-1]];
			if (f[i] < 0) f[i] = 0.0;
			if (f[i] > 1.0) f[i] = .999999999999999;
			h[i] = (int)floor(f[i]*H);
		}
		ff = f[W-1] +w[W-1][h[W-1]];
	}

	void backward(real x)
	{
		for (int i = 0; i < W; i++)
		{
			w[i][h[i]] -= rate*x;
		}
	}

The h[] array tracks the sequence of bins the input belongs to as it passes through the layers. This is necessary so that backprop can adjust the correct parameters. As implied by the name, we have f[i] = f[i-1]+w[i-1][h[i-1]], so the correct parameter (the one for that bin and layer) is simply added to the output from the last layer. And so on. 

https://www.youtube.com/watch?v=L1hwXWqsOIk

I may add more info later if I find the time or others find the project interesting.


