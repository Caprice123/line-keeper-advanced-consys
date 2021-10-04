import cv2
import utils
import numpy as np


image = cv2.imread("img_reg_lurus.jpg")
print(image.shape)
def increase_brightness(img, value=100):
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    h, s, v = cv2.split(hsv)

    lim = 255 - value
    v[v > lim] = 255
    v[v <= lim] += value

    final_hsv = cv2.merge((h, s, v))
    return final_hsv


def convert_to_HSV(img):
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    return hsv

def filter_color(img):
    lower_color = np.array([15, 0, 64], np.uint8)
    upper_color = np.array([111, 255, 255], np.uint8)
    mask = cv2.inRange(img, lower_color, upper_color)
    return mask

def detect_edges(img):
    edges = cv2.Canny (img, 50, 100)
    return edges

def roi(img):
    width, height = img.shape
    width, height = height, width
    mask = np.zeros_like(img)
    
    
    polygon = np.array([[
        (0, height), 
        (0,  height/2),
        (width , height/2),
        (width , height),
    ]], np.int32)
    
    cv2.fillPoly(mask, polygon, 255)
    crop = cv2.bitwise_and(img, mask)
    return crop

def detect_line_segments(img):
    rho = 1
    theta = np.pi / 180
    min_threshold = 0
    line_segments = cv2.HoughLinesP(img, rho, theta, min_threshold,
                                    np.array([]), minLineLength=80, maxLineGap=60)
    return line_segments

def make_points(img, line):
    height, width, _ = img.shape
    
    slope, intercept = line
    y1 = height
    y2 = int(y1/2)
    
    if slope == 0:
        slope = 0.1
        
    x1 = int((y1 - intercept) / slope)
    x2 = int((y2 - intercept) / slope)
    return [[x1,y1,x2,y2]]

def average_slope_intercept(img, line_segments):
    lane_lines = []
    
    if line_segments is None:
        print("no line segment detected")
        return lane_lines
    
    height, width, _ = img.shape
    
    left_fit = []
    right_fit = []
    boundary = 1/3
    
    left_region_boundary = width * (1 - boundary)
    right_region_boundary = width * boundary
    
    for line_segment in line_segments:
        for x1, y1, x2, y2 in line_segment:
            if x1 == x2:
                print("skipping vertical lines (slope = infinity)")
                continue
            slope = (y2 - y1) / (x2 - x1)
            intercept = y1 - (slope * x1)
            
            if (slope < 0):
                if (x1 < left_region_boundary and x2 < left_region_boundary):
                    left_fit.append((slope, intercept))
                    
            else:
                if (x1 > right_region_boundary and x2 > right_region_boundary):
                    right_fit.append((slope, intercept))

    left_fit_average = np.average(left_fit, axis=0)
    if len(left_fit) > 0:
        lane_lines.append(make_points(img, left_fit_average))
        
        
    right_fit_average = np.average(right_fit, axis=0)
    if len(right_fit) > 0:
        lane_lines.append(make_points(img, right_fit_average))
        
    return lane_lines


def display_lines(img, lines, line_color = (0,255,0), line_width=5):
    line_image = np.zeros_like(img)
    
    if (lines is not None):
        for line in lines:
            for x1,y1,x2,y2 in line:
                cv2.line(img, (x1, y1), (x2, y2), line_color, line_width)
                
    line_image = cv2.addWeighted(img, 0.8, line_image, 1, 1)
    return line_image


image = cv2.fastNlMeansDenoisingColored(image, None, 10, 10, 7, 21)
image = cv2.GaussianBlur(image, (5,5), 0)
imgBlank = np.zeros((240, 320, 3), np.uint8)
utils.initializeTrackbars(count=3)



while 1:
    
    image_copy = image.copy()
    image_hsv = convert_to_HSV(image_copy)
    filtered = filter_color(image_hsv)
    edges = detect_edges(filtered)
    crop = roi(edges)
    
    lines = detect_line_segments(crop)
    lane_lines = average_slope_intercept(image_copy, lines)
    lane_lines_image = display_lines(image_copy, lane_lines)
    
    
    imgArray = [
                    [image, image_copy, filtered],
                    [imgBlank, edges, crop]
                    ]
    
    
    
    stackImages = utils.stackImages(imgArray, 0.75)
    cv2.imshow('image', stackImages)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
        
        

cv2.waitKey(0)
cv2.destroyAllWindows()