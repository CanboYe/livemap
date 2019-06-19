#!/usr/bin/env python

# --------------------------------------------------------
# Faster R-CNN
# Copyright (c) 2015 Microsoft
# Licensed under The MIT License [see LICENSE for details]
# Modified by Kevin Christensen
# --------------------------------------------------------

"""
Vision Detection System
"""
import roslib
roslib.load_manifest('livemap_ros')
import _init_paths
from fast_rcnn.config import cfg
from fast_rcnn.test import im_detect
from fast_rcnn.nms_wrapper import nms
from utils.timer import Timer
import matplotlib.pyplot as plt
import numpy as np
import scipy.io as sio
import caffe
import os
import sys
import cv2
import argparse
import math


#import sys
import rospy
from std_msgs.msg import String
from std_msgs.msg import Int16
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError

RUNNING = False
IMAGE = None

CLASSES = ('__background__',
           'cone')

NET = {'cone': ('cone',
                   'model_iter_10000.caffemodel')}


def vis_detections(im, class_name, dets, thresh=0.5):
    """Draw detected bounding boxes."""
    inds = np.where(dets[:, -1] >= thresh)[0]
    if len(inds) == 0:
        return

    im = im[:, :, (2, 1, 0)]
    fig, ax = plt.subplots(figsize=(12, 12))
    #ax.imshow(im, aspect='equal')
    for i in inds:
        bbox = dets[i, :4]
        score = dets[i, -1]

        ax.add_patch(
            plt.Rectangle((bbox[0], bbox[1]),
                          bbox[2] - bbox[0],
                          bbox[3] - bbox[1], fill=False,
                          edgecolor='red', linewidth=3.5)
        )
        ax.text(bbox[0], bbox[1] - 2,
                '{:s} {:.3f}'.format(class_name, score),
                bbox=dict(facecolor='blue', alpha=0.5),
                fontsize=14, color='white')

    ax.set_title(('{} detections with '
                  'p({} | box) >= {:.1f}').format(class_name, class_name,
                                                  thresh),
                 fontsize=14)
    plt.axis('off')
    plt.tight_layout()
    plt.draw()


def demo(net, image_name):
    """Detect object classes in an image using pre-computed object proposals."""

    # Load the demo image
    im_file = os.path.join(cfg.DATA_DIR, 'demo', image_name)
    im = cv2.imread(im_file)

    # Detect all object classes and regress object bounds
    timer = Timer()
    timer.tic()
    scores, boxes = im_detect(net, im)
    timer.toc()
    print ('Detection took {:.3f}s for '
           '{:d} object proposals').format(timer.total_time, boxes.shape[0])

    # Visualize detections for each class
    CONF_THRESH = 0.95
    NMS_THRESH = 0.3
    for cls_ind, cls in enumerate(CLASSES[1:]):
        cls_ind += 1  # because we skipped background
        cls_boxes = boxes[:, 4*cls_ind:4*(cls_ind + 1)]
        cls_scores = scores[:, cls_ind]
        dets = np.hstack((cls_boxes,
                          cls_scores[:, np.newaxis])).astype(np.float32)
        keep = nms(dets, NMS_THRESH)
        dets = dets[keep, :]
        vis_detections(im, cls, dets, thresh=CONF_THRESH)


def detect_potholes(im):
    global NET
    # Load the demo image
    #im_file = os.path.join(cfg.DATA_DIR, 'demo', image_name)
    #im = cv2.imread(im_file)

    # Detect all object classes and regress object bounds
    timer = Timer()
    timer.tic()
    scores, boxes = im_detect(NET, im)
    timer.toc()
    print ('Detection took {:.3f}s for '
           '{:d} object proposals').format(timer.total_time, boxes.shape[0])
    with open('timingData.txt', 'a') as timingFile:
        timingFile.write('%.4f\n' % timer.total_time)
        #timingFile.close()

    # Visualize detections for each class
    CONF_THRESH = 0.95
    NMS_THRESH = 0.3
    totFlag = 0
    for cls_ind, cls in enumerate(CLASSES[1:]):
        cls_ind += 1  # because we skipped background
        cls_boxes = boxes[:, 4*cls_ind:4*(cls_ind + 1)]
        cls_scores = scores[:, cls_ind]
        dets = np.hstack((cls_boxes,
                          cls_scores[:, np.newaxis])).astype(np.float32)
        keep = nms(dets, NMS_THRESH)
        dets = dets[keep, :]
        im, flag = vis_potholes(im, cls, dets, thresh=CONF_THRESH)
        totFlag = totFlag + flag

    return im, totFlag


def vis_potholes(im, class_name, dets, thresh=0.5):
    """Draw detected bounding boxes."""
    inds = np.where(dets[:, -1] >= thresh)[0]
    flag = 1
    if len(inds) == 0:
        flag = 0
        return im, flag

    #im = im[:, :, (2, 1, 0)]
    #fig, ax = plt.subplots(figsize=(12, 12))
    #ax.imshow(im, aspect='equal')
    for i in inds:
        bbox = dets[i, :4].astype(int)
        score = dets[i, -1]
        cv2.rectangle(im, (bbox[0], bbox[1]), (bbox[2],
                                               bbox[3]), color=(0, 0, 255), thickness=2)
        font = cv2.FONT_HERSHEY_SIMPLEX
        cv2.rectangle(im, (bbox[0], bbox[1]-10), (bbox[0]+90,
                                                  bbox[1]), color=(255, 0, 0), thickness=-1)
        cv2.putText(im, '{:s} {:.3f}'.format(class_name, score), (
            bbox[0], bbox[1]-2), fontFace=font, fontScale=0.4, color=(255, 255, 255), thickness=1, lineType=cv2.LINE_AA)

    # ax.set_title(('{} detections with '
     #             'p({} | box) >= {:.1f}').format(class_name, class_name,
    #                                              thresh),
     #             fontsize=14)
    # plt.axis('off')
    # plt.tight_layout()
    # plt.draw()
    # cv2.imwrite('test1.png',im)
    return im, flag


def init_net(prototxt, caffemodel, labelfile, cfg, cpu_mode=False, gpu_id=0):
    cfg.TEST.HAS_RPN = True  # Use RPN for proposals
    # if cpu_mode:
    #    caffe.set_mode_cpu()
    # else:
    caffe.set_mode_gpu()
    caffe.set_device(gpu_id)
    cfg.GPU_ID = gpu_id
    net = caffe.Net(prototxt, caffemodel, caffe.TEST)
    print '\n\nLoaded network {:s}'.format(caffemodel)
#    classes=read_in_labels(labelfile)
    return net, CLASSES  # tuple(classes)


def read_in_labels(fp):
    if fp and os.path.isfile(fp):
        with open(fp, 'r') as f:
            labels = f.read().splitlines()
        labels.insert(0, '__background__')
        return labels
    else:
        raise IOError('read from labelfile {} failed!'.format(fp))


def callback(im):
    global IMAGE
    global RUNNING
    rospy.loginfo('Image received')
    if (RUNNING):
        rospy.logwarn('Detection already running, message omitted')
    else:
        RUNNING = True
    IMAGE = im

def IoU(gt,det):
    xA = max(gt[0],det[0]) #max of min
    xB = min(gt[2],det[2]) #min of max
    yA = max(gt[1],det[1])
    yB = min(gt[3],det[3])

    intA = max(0, (xB-xA+1.0)*(yB-yA+1.0))
    boxA = (gt[0]-gt[1]+1)*(gt[2]-gt[3]+1)
    boxB = (det[2]-det[0]+1)*(det[3]-det[1]+1)

    iou = intA/(boxA+boxB-intA)

    return iou

def measureDist(lat1, lon1, lat2, lon2): # 
    R = 6378.137 # Radius of earth in KM
    dLat = lat2 *math.pi / 180.0 - lat1 * math.pi / 180.0
    dLon = lon2 * math.pi / 180.0 - lon1 * math.pi / 180.0
    a = math.sin(dLat/2.0) * math.sin(dLat/2.0) + math.cos(lat1 * math.pi / 180.0) * math.cos(lat2 * math.pi / 180.0) *math.sin(dLon/2.0) * math.sin(dLon/2.0)
    c = 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))
    d = R * c
    return d * 1000.0 # meters

if __name__ == '__main__':
    prototxt = '/home/cloudlet/classifier3/faster_rcnn_test.pt'
    caffemodel = '/home/cloudlet/classifier3/model_iter_50000.caffemodel'
    #im = 'frame425.jpg'

    cpu_mode = False
    gpu_id = 0
    if not os.path.isfile(caffemodel):
        raise IOError(('{:s} not found.\nDid you run ./data/script/'
                       'fetch_faster_rcnn_models.sh?').format(caffemodel))

    image_name = 'frame109.jpg'
    im_file = os.path.join(cfg.DATA_DIR, 'demo', image_name)
    im = cv2.imread(im_file)

    NET, classes = init_net(prototxt, caffemodel, CLASSES, cfg, cpu_mode=False)

    image_pub = rospy.Publisher("image_detected", Image, queue_size=1)
    flag_pub = rospy.Publisher("detect_flag",Int16, queue_size=1)
    bridge = CvBridge()
    #image_sub = rospy.Subscriber("usb_cam/image_raw", Image, callback, queue_size=6, buff_size=2**24)
    image_sub = rospy.Subscriber("camera/image_raw", Image, callback, queue_size=1, buff_size=2**24)

    rospy.init_node('coneDetector', anonymous=False)

    detected_img, flag = detect_potholes(im)
    lastImage = None
    r = rospy.Rate(30)
    
    framesSent = 0
    while not rospy.is_shutdown():
        if (RUNNING):
            r.sleep()
            cv_image = bridge.imgmsg_to_cv2(IMAGE, "bgr8")
            detected_img, sendFlag = detect_potholes(cv_image)
            if image_pub.get_num_connections() > 0 and sendFlag != 0:
                image_pub.publish(bridge.cv2_to_imgmsg(detected_img, "bgr8"))
                #sentFile = '{}{}{}{}'.format(sentFilesDir,"/frame",int(imNum),".jpg")
                #cv2.imwrite(sentFile,detected_img)
                framesSent+=1
                
                # If we detect something, set a flag that we did
                flag_pub.publish(1)
                lastImage = detected_img
            else:
                # Otherwise, don't send 0 and show last image
                flag_pub.publish(0)
                #if lastImage is not None:
                    #image_pub.publish(bridge.cv2_to_imgmsg(lastImage, "bgr8"))
            RUNNING = False
        else:
            r.sleep()

    # cv2.imwrite('file.png',img)