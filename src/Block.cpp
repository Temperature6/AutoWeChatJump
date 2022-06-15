#include "Block.h"

Block::Block(string img_path)
{
	file_path = img_path;
	block_img = imread(file_path);
	if (block_img.empty())
	{
		throw NoSuchFile;
	}
}
