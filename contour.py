import numpy as np
import json
import math
import cv2
import sys


onlyLines = False
def processPoints(im, debug):

  # Convert to Grayscale
  imgray = cv2.cvtColor(im,cv2.COLOR_BGR2GRAY)
  
  # Threshold the image with adaptative thresholding
  thresh = cv2.adaptiveThreshold(imgray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 15, 2)

  # Find image contours
  (_, cnts, _) = cv2.findContours(thresh.copy(), cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

  # Output of "good" contours
  allcnts = []

  # Auxiliarry variables to help understand what is being processed (total points)
  totalP = 0

  # loop over our contours
  for c in cnts:
    # approximate the contour
    peri = cv2.arcLength(c, True)

    # If perimeter is too low, discart contour
    if peri < 100:
      continue

    # Try approximating poligon to simplify number of points
    approx = cv2.approxPolyDP(c, 3, True)

    # Sum total points processed
    totalP = totalP + len(approx)

    # Append contour to "good contours"
    allcnts.append(approx)

  # Debug total points
  print("Frame total points: " + str(totalP))

  # Debug: Show image contours
  if debug: 
    if onlyLines:
      out = im.copy() * 0
      cv2.drawContours(out, allcnts, -1, (0,255,0), 1)
    else:
      out = im

    # Debug: Show image
    cv2.imshow("thres", thresh)
    cv2.imshow("output", out)

  # Stored list of x y points in series
  points = []

  # Loop over selected contours and append everything to a single array of x, y, x, y, x, y... coordinates
  for contour in allcnts:
    for p in contour:
      x = p[0][0] * 1.0
      y = p[0][1] * 1.0
      xOut = int(math.floor(x / im.shape[0] * 255))
      yOut = int(math.floor(y / im.shape[1] * 255))
      points.append(xOut)
      points.append(yOut)

  # Return points
  return points

def savePoints(points, name):
  f = open('streams/' + name + '.json', 'w')
  f.write(json.dumps(points))
  f.close()

# im = cv2.imread('bat.png')

debug = False
fileName = 'beer.mp4'
if len(sys.argv) >= 2:
  fileName = sys.argv[2]

file = 'media/' + fileName
name = fileName.split('.')[0]

print()
print(' File input: ' + file)
print('  File Name: ' + name)
print('')

cap = cv2.VideoCapture(file)

# Stored list of frames
frames = []

# Flag indicating if it is recording or not
recording = True
while True: # cap.isOpened()):
    ret, frame = cap.read()

    if ret == False:
        cap.set(2 , 0)

        if recording:
          # Stop point saving
          recording = False

          # Output json data
          print('Saving ' + str(len(frames)) + ' frames')
          savePoints(frames, name)

          break;

        print("reseting frame")
        continue;

    points = processPoints(frame, debug)

    # Append frame to frames (will save everything later)
    if recording:
      frames.append(points)

    # Debug: Listen to inputs
    if debug:
      # Check for user keys input
      key = cv2.waitKey(debug) & 0xFF
      if key == ord('q'):
          break

      # Toggle showing lines or video
      if key == ord('t'):
        onlyLines = not onlyLines

# Release video and exit
cap.release()
cv2.destroyAllWindows()