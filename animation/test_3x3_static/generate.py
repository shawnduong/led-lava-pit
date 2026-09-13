#!/usr/bin/env python3

import argparse
import re
import matplotlib.pyplot as plt
import numpy as np

OUTPUT   = "test_3x3_static.dat"
TEMPLATE = "template.h"
HEADER   = "data.h"

RED   = (255,   0,   0)
GREEN = (  0, 255,   0)
BLUE  = (  0,   0, 255)

DIMENSIONS = (3, 3)
KEYFRAMES  = 1

def generate_pixel_array(output_file):

	pixels = [
		RED  , RED  , RED  ,
		GREEN, GREEN, GREEN,
		BLUE , BLUE , BLUE ,
	]

	with open(output_file, "wb") as f:
		for pixel in pixels:
			f.write(bytes(pixel))

def generate_header(input_file, template_file, output_file):

	with open(input_file, "rb") as f:
		data = f.read()

	# Format bytes as a comma-separated C array body, wrapped at a reasonable width.
	byte_strs = [f"0x{b:02x}" for b in data]
	lines = []
	for i in range(0, len(byte_strs), 12):
		lines.append("\t" + ", ".join(byte_strs[i:i+12]) + ",")
	array_body = "\n".join(lines)

	with open(template_file, "r") as f:
		template = f.read()

	directive = re.compile(r"#PLACEHOLDER")
	if not directive.search(template):
		raise ValueError(f'No "#PLACEHOLDER" directive found in {template_file}')
	header = directive.sub(array_body, template)

	with open(output_file, "w") as f:
		f.write(header)

def preview(input_file, dimensions, keyframes):

	with open(input_file, "rb") as f:
		data = f.read()
	pixels = np.frombuffer(data, dtype=np.uint8).reshape((keyframes, *dimensions, 3))

	plt.imshow(pixels[0])
	plt.xticks(np.arange(-0.5, dimensions[1], 1), np.arange(0, dimensions[1]+1, 1))
	plt.yticks(np.arange(-0.5, dimensions[0], 1), np.arange(0, dimensions[0]+1, 1))
	plt.grid(color="black", linestyle="-", linewidth=1)
	plt.gca().xaxis.tick_top()
	plt.gca().format_coord = lambda x, y: ""
	plt.show()

def main():

	p = argparse.ArgumentParser(
		description="Generate a 3x3 static test pixel array.")
	p.add_argument(
		"--preview",
		action="store_true",
		help="Preview the generated pixel array after generation.")
	p.add_argument(
		"--output",
		type=str,
		default=OUTPUT,
		help=f"Output file name (default: {OUTPUT})")
	p.add_argument(
		"--template",
		type=str,
		default=TEMPLATE,
		help=f"Template header file (default: {TEMPLATE})")
	p.add_argument(
		"--header",
		type=str,
		default=HEADER,
		help=f"Output header file (default: {HEADER})")
	args = p.parse_args()

	print(":: Generating 3x3 static pixel array.")
	data = generate_pixel_array(args.output)
	print(f"   Pixel array written to {args.output}")

	print(":: Generating header from template.")
	generate_header(args.output, args.template, args.header)
	print(f"   Header written to {args.header}")

	if args.preview:
		print(":: Previewing pixel array.")
		preview(args.output, DIMENSIONS, KEYFRAMES)

	print(":: Done.")

if __name__ == "__main__":
	main()
