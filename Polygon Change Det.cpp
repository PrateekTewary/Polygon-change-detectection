#include<bits/stdc++.h>

using namespace std;

int no_of_points, no_of_new_points;
double centroid_x, centroid_y;
//Initial Polygon
vector<vector<double>> initial_poly_points;     //vector<tuple<2,double>>
//Line joining vertices to centroid
//Equation of point of initial polygon(P) to the centroid(G) of the form ax+by=c with slope of the equation( m=-b/a)
// a  b  c  m
vector<vector<double>> vertex_to_centre;
//New polygon to which change is to be detected
vector<vector<double>> new_poly_points1;
//Sides of new Polygon
// Coefficients of the side ( ax+by=c ) along with min max of each x and y coordinate
// a  b  c  min_x  max_x  min_y  max_y
vector<vector<double>> poly_sides;      //vector<tuple<3,double>>
//deviation from initial_poly_points
vector<vector<pair<double,double>>> deviation;
//inputs polygon
vector<double> deviation_magnitude;
void input( int& points, vector<vector<double>>& poly_point ){

    cout << "Total points in polygon: ";
    cin >> points;


    //Storing x and y coordinate of polygon
    for( int i = 0; i < points; ++i ){
        double x_cor, y_cor;
        cin >> x_cor >> y_cor;
        poly_point.push_back({x_cor,y_cor});
    }
}
//Computes equation of line passing through two points
//                              (x1,y1)           (x2,y2)
vector<double> compute_sides( vector<double>& P, vector<double>& Q ){

    double x1, y1, x2, y2;
    x1 = P[0];
    y1 = P[1];
    x2 = Q[0];
    y2 = Q[1];

    //equation of form --> ax+by = c
    double a = -(y2-y1), b = (x2-x1), c = (x2-x1)*y1-(y2-y1)*x1;

    return {a,b,c};

}

void compute_vetex_to_centroid(){
    vector<double> Centre = {centroid_x,centroid_y};
    for( int i = 0; i < no_of_points; ++i ){

        vector<double> line_centroid_to_initialPt = compute_sides( initial_poly_points[i], Centre );
        double x0 = initial_poly_points[i][0];
        double x1 = centroid_x;
        double y0 = initial_poly_points[i][1];
        double y1 = centroid_y;

        double x_min = min(x0,x1);
        double x_max = max(x0,x1);
        double y_min = min(y0,y1);
        double y_max = max(y0,y1);

        line_centroid_to_initialPt.push_back( x_min );
        line_centroid_to_initialPt.push_back( x_max );
        line_centroid_to_initialPt.push_back( y_min );
        line_centroid_to_initialPt.push_back( y_max );

        vertex_to_centre.push_back( line_centroid_to_initialPt );
    }

}
void compute_centroid(){
    double x_cor_sm = 0.0, y_cor_sm = 0.0;
    for( auto& points : initial_poly_points )
        x_cor_sm += points[0],
        y_cor_sm += points[1];

    centroid_x = x_cor_sm/(double)no_of_points;
    centroid_y = y_cor_sm/(double)no_of_points;
}
//compute the sides of the new polygon
void compute_new_polygon_sides(){
    double x1, y1, x2, y2, x_min, x_max, y_min, y_max;
    for( int i = 0; i < no_of_new_points; ++i ){
        x1 = new_poly_points1[i][0];
        y1 = new_poly_points1[i][1];
        x2 = new_poly_points1[(i+1)%no_of_new_points][0];
        y2 = new_poly_points1[(i+1)%no_of_new_points][1];

        x_min = min(x1,x2);
        x_max = max(x1,x2);
        y_min = min(y1,y2);
        y_max = max(y1,y2);

        vector<double> side;
        side = compute_sides( new_poly_points1[i], new_poly_points1[(i+1)%no_of_new_points] );
        //include limits, i.e, the range in which side is defined(makes the line a line segment) so that the solution can be validated
        // valid solution iff   min{x1,x2}<= x_sol <=max{x1,x2}     AND     min{y1,y2} <= y_sol <= max{y1,y2}
        side.push_back( x_min );
        side.push_back( x_max );
        side.push_back( y_min );
        side.push_back( y_max );

        poly_sides.push_back( side );
    }
}
//Solution of equation of two var (CRAMER'S RULE)
//https://stackoverflow.com/questions/19619248/solve-system-of-two-equations-with-two-unknowns
pair<double,double> solve_equation( vector<double>& P, vector<double>& Q ){
    /* solve the linear system
     * ax+by=e
     * cx+dy=f
     */
     //equation 1
     double a,b,e;
     a = P[0], b = P[1], e = P[2];
     //equation 2
     double c,d,f;
     c = Q[0], d = Q[1], f = Q[2];

    double determinant = a*d - b*c;
    if(determinant == 0.0)    //NO solution
        return {DBL_MIN,DBL_MIN};

    double x0 = (e*d - b*f)/determinant;
    double y0 = (a*f - e*c)/determinant;

    return {x0,y0};
}
//function to check if the solution is in valid range
//solution = {x0,y0} and a solution is in valid range when-> x_min<=x0<=x_max and y_min<=y0<=y_max
//NO, DOESN'T WORK ->Also the slope of the lines GP and GK should be equal( G-centroid, P-point of intersection, K-initial Point)
bool SolutionInRange( pair<double,double>& solution, double x_min1, double x_max1, double y_min1, double y_max1
                                                    ,double x_min2, double x_max2, double y_min2, double y_max2 ){
    double x0 = solution.first, y0 = solution.second;
    bool valid1 = x_min1 <= x0 && x0 <= x_max1 && y_min1 <= y0 && y0 <= y_max1;
    bool valid2 = x_min2 <= x0 && x0 <= x_max2 && y_min2 <= y0 && y0 <= y_max2;

    return valid1 && valid2;
}
void compute_deviation(){

    deviation = vector<vector<pair<double,double>>>( vertex_to_centre.size() );
    for( int i = 0, lines = vertex_to_centre.size(); i < lines; ++i ){

        double x_min2 = vertex_to_centre[i][3];
        double x_max2 = vertex_to_centre[i][4];
        double y_min2 = vertex_to_centre[i][5];
        double y_max2 = vertex_to_centre[i][6];
        pair<double,double> solution;
        for( int j = 0, no_of_side = poly_sides.size(); j < no_of_side; ++j ){

            solution = solve_equation( vertex_to_centre[i], poly_sides[j] );
            if( SolutionInRange( solution, poly_sides[j][3], poly_sides[j][4], poly_sides[j][5], poly_sides[j][6]
                                ,x_min2, x_max2, y_min2, y_max2 ) )
                deviation[i].push_back(solution);
        }
    }
}
double distance( double x0, double y0, double x1, double y1 ){
    return sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) );
}
//change definition of code for multiple points of intersection P ( here magnitude vector is linear but multiple intersection points possible
//+ve and -ve deviation to be added as well
void compute_deviation_magnitude(){
    for( int i = 0; i < no_of_points; ++i ){
        if( deviation[i].size() == 0 ){
            deviation_magnitude.push_back( DBL_MIN );
            continue;
        }

        double x0 = initial_poly_points[i][0];
        double y0 = initial_poly_points[i][1];
        //considering only one point of intersection for now
        double x1 = deviation[i][0].first;
        double y1 = deviation[i][0].second;
        deviation_magnitude.push_back( distance(x0,y0,x1,y1) );
    }
}
void print(){
    cout << "Number of points in initial polygon : " << no_of_points;
    cout << endl;
    cout << endl;
    for( auto& p : initial_poly_points )    cout << "(" << p[0] << "," << p[1] << ")  ";
    cout << endl;
    cout << endl;
    cout << "Centroid for the given polygon is (" << centroid_x << "," << centroid_y << ")";
    cout << endl;

    cout << endl;
    cout << "Equation of line from vertex to centroid: ";
    cout << endl;
    for( auto& x : vertex_to_centre ){
        cout << "\t" << x[0] << "  " << x[1] << "  " << x[2] << "   " << "      "  << x[3] << " " << x[4] << " " << x[5] <<
                " " << x[6] << endl;
    }
    cout << endl;
    cout << endl;

    cout << "Equation of sides : ";
    cout << endl;
    for( auto& s : poly_sides ){
        cout << "\t" << s[0] << "  " << s[1] << "  " << s[2] << "   |" << s[3] << " " << s[4]
                                                             << "|  |" << s[5] << " " << s[6] << "|";
        cout << endl;
    }
    cout << endl;
    cout << endl;

    cout << "Point of intersection from each point of initial polygon to centroid with the sides of new polygon:";
    cout << endl;
    for( int i = 0; i < no_of_points; ++i ){
        cout << "\t" << (i+1) << "th point intersects - ";
        for( auto p : deviation[i] ){
            cout << "   (" << p.first << "," << p.second << ")";
        }
        cout << endl;
    }
    cout << endl;
    cout << endl;

    cout << "Deviations are :";
    cout << endl;
    for( int i = 0; i < no_of_points; ++i ){
        cout << "\t\t" << "Deviation from " << (i+1) << "th point - ";
        cout << deviation_magnitude[i];
        cout << endl;
    }
}

int main(){

    //input initial polygon
    input(no_of_points,initial_poly_points);

    compute_centroid();

    compute_vetex_to_centroid();

    //input new polygon the change of which is to be measured w.r.t initital poly
    input(no_of_new_points,new_poly_points1);

    compute_new_polygon_sides();

    compute_deviation();

    compute_deviation_magnitude();
    print();

    return 0;
}

//INPUT 2
/*
5
3 3
-1.02 1.58
1 -2
7 -1
7 2
5
2.56 2.04
1 1
1.42 -1.34
4.82 -1.1
5 1
*/
