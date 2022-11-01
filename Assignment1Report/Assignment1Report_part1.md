

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
	// TODO: Implement bresenham algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

		int	x0 = p1.x, x1 = p2.x, y0 = p1.y, y1 = p2.y; 
		const int distanceY = -abs(y1 - y0), distanceX = abs(x1 - x0);
		const int setX = x0 < x1 ? 1 : -1; //set the direction for the line
		const int setY = y0 < y1 ? 1 : -1; //set the direction for the line
		int error = distanceX + distanceY;
		bool flagX ,flagY;
		flagX = x0 != x1 ? false : true;
		flagY = y0 != y1 ? false : true;
		while (!flagX||!flagY) // while the line isn't at p2
		{
			PutPixel(x0, y0, color);
				int e2 = 2 * error;
				if (e2 >= distanceY) { 
					if (!flagX)
					{
						error += distanceY;
						x0 += setX;
					}
				}
				else
				{
					if (e2 <= distanceX)
					{
						if (!flagY)
						{
							error += distanceX;
							y0 += setY;
						}
					}
				}
			if (x0 == x1) flagX = true;
			if (y0 == y1) flagY = true;
		}
}


void Renderer::Render(const Scene& scene)
{
	// TODO: Replace this code with real scene rendering code
	
	const int x0 = viewport_width / 2;
	const int y0 = viewport_height / 2;
	glm::ivec2 center = glm::ivec2(x0, y0);
	glm::vec3 color = glm::vec3(1.0f, 0.5f, 0.31f);
	const int radius = min(x0, y0) / 2;
	const int stepSize = 360;
	
	// draw circle

	for (int i = 0;i < 360;i++)
	{
		double x1 = x0 + radius * (sin((2 * M_PI * i)/stepSize));
		double y1 = y0 + radius * (cos((2 * M_PI * i) / stepSize));
		DrawLine(center, glm::ivec2(x1,y1), color);
	}
	
}

<img src="./circle.png">