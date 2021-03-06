//
//  objectness.cpp
//  ProductGrade
//
//  Created by Joseph lefebvre on 2018-04-16.
//  Copyright © 2018 Joseph lefebvre. All rights reserved.
//


#include "objectness.hpp"


using namespace cv ;

std::pair<int, int> nativeResolution(1440,900) ;

String path_objectnessVis = "/Users/josephlefebvre/Honours_Project/Demo/" ;

int objectness(Mat& cannyEdges, Mat& sobelMagnitude, Rect bound_box, Point offset, Mat& src_original){
    std::vector<std::vector<Point>> contours ;
    std::vector<Vec4i> hierarchyCannyContours ;
    int perimeter_boundbox = (bound_box.width*2) + (bound_box.height*2) ;
    //std::cout << "Perimeter of box: " << perimeter_boundbox << std::endl ;
    int score = 0;
    
    findContours(cannyEdges, contours, hierarchyCannyContours, CV_RETR_LIST,CV_CHAIN_APPROX_NONE ) ;
    Mat contoursCanny = Mat::zeros(cannyEdges.rows,cannyEdges.cols,CV_8U) ;
    
    Mat contoursCannyNoCross = Mat::zeros(src_original.rows,src_original.cols,CV_8U) ;
    //drawContours(contoursCanny, contours, -1, Scalar(255,0,0)) ;
    cvtColor(contoursCanny, contoursCanny, CV_GRAY2BGR) ;
    cvtColor(contoursCannyNoCross, contoursCannyNoCross, CV_GRAY2BGR) ;
    //line(contoursCanny, Point(0,0), Point(100,100), Scalar(0,255,0)) ;
    // Attempt to use arc length to get length of contours
    //    for (int i = 0 ; i < contours.size() ; ++i){
    //        double lengthContour = arcLength(Mat(contours[i]), false) ;
    //        //circle(contoursCanny, contours[i][0], 3, Scalar(0,255,0),1,8) ;
    //        std::cout << "length of contour " << i << ": with starting point: "
    //        << contours[i][0] << lengthContour << std::endl ;
    //
    ////          imshow("find_canny_contours",contoursCanny) ;
    ////        waitKey() ;
    ////        destroyWindow("find_canny_contours") ;
    //    }
    std::vector<std::vector<Point>> contoursWithOutCrossings ;
    int scoreIndivContour ;
    
    //namedWindow("bound_box") ;
    Mat copy ;
    Mat copy2 ;
    src_original.copyTo(copy);
    src_original.copyTo(copy2);
    rectangle(copy, bound_box, Scalar(0,255,0)) ;
    //imshow("bound_box",copy) ;
    // imshow("contours",contoursCanny) ;
    
    //    for(int i = 0 ; i < contours.size() ; ++ i){
    //        for(int j = 0 ; j < contours[i].size(); ++j){
    //            Point pointOnContour = contours[i][j] ;
    //            circle(copy, pointOnContour, 1, Scalar(0,0,255));
    //            imshow("points",copy) ;
    //
    //
    //        }
    //    }
    //    waitKey() ;
    
    namedWindow("points");
    moveWindow("points", nativeResolution.first-copy.cols, 0) ;
    
    //    namedWindow("Canny_Contours");
    //    moveWindow("Canny_Contours", nativeResolution.first/2, 0) ;
    //    imshow("Canny_Contours", contoursCanny) ;
    namedWindow("Canny_without_cross_onBLACK");
    moveWindow("Canny_without_cross_onBLACK", nativeResolution.first - copy.cols, copy.rows + 45) ;
    bool flag_score = true ;
    int currentMax_IndivContour = 0 ;
    int currentMin_IndivContour = 0 ;
    
    for(int i = 0 ; i < contours.size() ; ++ i){
        // push in the contour and initalize score of individual contour
        scoreIndivContour = 0 ;
        
        contoursWithOutCrossings.push_back(contours[i]);
        //std::cout << "A: CONTOURs remaining " << contoursWithOutCrossings.size() << std::endl ;
        for(int j = 0 ; j < contours[i].size(); ++j){
            char skipContour  ;
            //            if(skipContour == (int) 'n'){
            //                return 0 ;
            //            }
            // Filter any contours that touch the boundary of the bounding box i.e. are crossing the edge
            Point pointOnContour = contours[i][j] ;
            Point pointOnContour_relativeToLanes(pointOnContour.x
                                                 +offset.x,pointOnContour.y + offset.y) ;
            int sobel_at_point = sobelMagnitude.at<uchar>(pointOnContour_relativeToLanes) ;
            
            //std::cout << "On contour Id " << i << " @ " << pointOnContour << "Magnitude: " << (int) sobelMagnitude.at<uchar>(pointOnContour) <<  std::endl ;
            
            if (! bound_box.contains(pointOnContour_relativeToLanes) ){
                //std::cout << "Point@" << pointOnContour_relativeToLanes << "Sobel is " << sobel_at_point << std::endl ;
                // if the current point has gradient which less then 20% of current maxium , do not discard contour rightaway
                // even though this point lies outside the boundary of the bounding box.
                if(sobel_at_point < currentMax_IndivContour/5){
                    //                    std::cout << "Triggered @" << pointOnContour_relativeToLanes << "Sobel is " << sobel_at_point <<
                    //                    "currentMax is @ " << currentMax_IndivContour << std::endl ;
                    // Keep score of contour, don't remove if the point on the contours
                    // that is crossing the bounding box is of very low magnitue.
                    // will account for shadows and other small noise
                    flag_score = true ;
                    continue ;
                }
                circle(copy,pointOnContour_relativeToLanes,1, Scalar(0,0,255) ) ;
                //n        imshow("points",copy) ;
                
                // ignore this contour line and remove from
                // contours without crossings vector
                //circle(contoursCannyNoCross, contours[i][0], 3, Scalar(0,255,0),1,8) ;
                scoreIndivContour = 0 ; // Restet counter to zero for this contour
                //std::cout << "B: CONTOURs remaining " << contoursWithOutCrossings.size() << std::endl ;
                //                if(! contours.empty()){
                //                    contoursWithOutCrossings.pop_back() ;
                //                }
                //std::cout << "/***\tREMOVING CONTOUR " << i << std::endl ;
                //circle(copy2, pointOnContour_relativeToOriginal, 1, Scalar(0,0,255));
                //                namedWindow("not valid point") ;
                //                moveWindow("not valid point", nativeResolution.first -copy2.cols, 0);
                //imshow("not valid point",copy2) ;
                //waitKey() ;
                //std::cout << "\twith starting point: " << contours[i][0] << std::endl ;
                // skip this contour line and move on to the nexth
                flag_score = false ;
                
                
            }else{
                if(flag_score ){
                    if (sobel_at_point > currentMax_IndivContour){
                        currentMax_IndivContour = sobel_at_point ;
                    }
                    if(sobel_at_point < currentMin_IndivContour){
                        currentMax_IndivContour = sobel_at_point ;
                    }
                    circle(copy, pointOnContour_relativeToLanes, 1, Scalar(0,255,0));
                    scoreIndivContour += sobelMagnitude.at<uchar>(pointOnContour_relativeToLanes) ;
                }else {
                    circle(copy, pointOnContour_relativeToLanes, 1, Scalar(255,0 ,0));
                }
            }
            //            imshow("points",copy) ;
            //            waitKey(1) ;
        }
        if(! flag_score){
            contoursWithOutCrossings.pop_back() ;
        }
        
        flag_score = true ;
        
        
        //waitKey(1) ;
        score += round(scoreIndivContour/perimeter_boundbox)  ;
    }
    //waitKey() ;
    //    drawContours(copy, contoursWithOutCrossings, -1, Scalar(0,0,255), 1,
    //
    //                 8, std::vector<Vec4i>(), INT_MAX, Point() );
    drawContours(contoursCannyNoCross, contoursWithOutCrossings, -1, Scalar(0,255,0), 1,
                 8, std::vector<Vec4i>(), INT_MAX, offset );
    
    imshow("Canny_without_cross_onBLACK",contoursCannyNoCross) ;
    //    namedWindow("Canny_without_cross");
    //    moveWindow("Canny_without_cross", 0, nativeResolution.second - copy2.rows) ;
    putText(copy, std::to_string(score), bound_box.br(), FONT_HERSHEY_PLAIN, 1, Scalar(0,255,0)) ;
    //
    
    imshow("points",copy) ;
    
    
    
    
    
    
    
    
    //    imshow("Canny_without_cross",copy) ;
    return score ;
}


