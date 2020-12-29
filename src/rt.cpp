#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <inttypes.h>
#include <sstream>
#include <string>
#include <string.h>
#include <fstream>

#include <log.h>

/* ============================================================================
 * DEFINITIONS
 * ============================================================================
 */

#define SKY_COLOR vect(10,10,10)
#define ARR_LEN(x) (sizeof(x)/sizeof(x[0]))
#define FP_COMP(a,b,t) (abs((a) - (b)) < (t))

#ifndef M_PI
#define M_PI 3.141592658f
#endif
/* ============================================================================
 * CLASSES
 * ============================================================================
 */

class vect {
public:
    float x, y, z;
    vect() : x(0), y(0), z(0) {}
    vect(float a, float b, float c)
    {
        x = a;
        y = b;
        z = c;
    }

    vect operator+(vect o)
    {
        return vect(x + o.x, y + o.y, z + o.z);
    }
    void operator+=(vect o)
    {
        *this = *this + o;
    }

    vect operator-(vect o)
    {
        return vect(x - o.x, y - o.y, z - o.z);
    }

    vect operator*(float k)
    {
        return vect(x * k, y * k, z * k);
    }

    vect operator/(float k)
    {
        return vect(x / k, y / k, z / k);
    }

    // dot product
    float operator%(vect o)
    {
        return x * o.x + y * o.y + z * o.z;
    }

    // cross product (hopefully)
    vect operator^(vect o)
    {
        return vect(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
    }

    // normalize
    vect operator!()
    {
        return *this * (1 / sqrt(*this % *this));
    }

    float mag()
    {
        return sqrt(*this % *this);
    }
};

typedef struct {
    vect dir, origin;
} ray_t;

class scene_obj {
public:
    vect col_rgb;
    float reflect;
    vect loc;

    // abstract function
    // returns distance if intersect
    // negative number if no intersect
    float intersect(vect dir, vect origin) {
        return 0;
    }
};

struct sphere : scene_obj {
public:
    float radius;
    sphere(vect l, float r, vect c, float m) { loc = l; radius = r; col_rgb = c; reflect = m;}

    // https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
    float intersect(vect dir, vect origin) {
        if ( !FP_COMP(dir%dir, 1, 0.001) ) {
            printf("ERROR: non-unit direction vector");
            return -1;
        } 

        vect o_c = origin - loc;
        float a = dir % o_c;
        float del = a*a - (o_c%o_c - (radius*radius));
        if (del < 0) return del;
        float d = -1* a - sqrt(del);
        //TODO: check for d < 0
        return d;
    }
};

/* ============================================================================
 * STATIC VARIABLES
 * ============================================================================
 */

static bool m_debug;
const vect cam_pos = vect(0,0,1.5);
const vect cam_dir = !vect(0,0,1);
sphere scene[] = {
    sphere(vect(0,0.1,2.3), 0.01, vect(200,200,100), 0.1),
    sphere(vect(0,0.1,2.4), 0.01, vect(200,200,120), 0.1),
    sphere(vect(0,0.1,2.5), 0.01, vect(200,200,140), 0.1),
    sphere(vect(0,0.1,2.6), 0.01, vect(200,200,160), 0.1),
    sphere(vect(0,0.1,2.7), 0.01, vect(200,200,180), 0.1),
    sphere(vect(0,0.1,2.8), 0.01, vect(200,200,200), 0.1),
    sphere(vect(-1,-1,3), 0.5, vect(200,0,100), 0.5),
    sphere(vect(-1,-1,4), 0.5, vect(180,0,100), 0.8),
    sphere(vect(-1,-1,5), 0.5, vect(160,0,100), 0.5),
    sphere(vect(-1,-1,6), 0.5, vect(140,0,100), 0.5),
    sphere(vect(-1,-1,7), 0.5, vect(120,0,100), 0.5),
    sphere(vect(-1,-1,8), 0.5, vect(100,0,100), 0.5),
    sphere(vect(-1,-1,9), 0.5, vect(80,0,100), 0.5),
    sphere(vect(-1,-1,10), 0.5, vect(60,0,100), 0.5),
    sphere(vect(-1,1,3), 0.5, vect(200,0,100), 0.5),
    sphere(vect(-1,1,4), 0.5, vect(180,0,100), 0.8),
    sphere(vect(-1,1,5), 0.5, vect(160,0,100), 0.5),
    sphere(vect(-1,1,6), 0.5, vect(140,0,100), 0.5),
    sphere(vect(-1,1,7), 0.5, vect(120,0,100), 0.5),
    sphere(vect(-1,1,8), 0.5, vect(100,0,100), 0.5),
    sphere(vect(-1,1,9), 0.5, vect(80,0,100), 0.5),
    sphere(vect(-1,1,10), 0.5, vect(60,0,100), 0.5),
    sphere(vect(1,-1,3), 0.5, vect(200,0,100), 0.5),
    sphere(vect(1,-1,4), 0.5, vect(180,0,100), 0.8),
    sphere(vect(1,-1,5), 0.5, vect(160,0,100), 0.5),
    sphere(vect(1,-1,6), 0.5, vect(140,0,100), 0.5),
    sphere(vect(1,-1,7), 0.5, vect(120,0,100), 0.5),
    sphere(vect(1,-1,8), 0.5, vect(100,0,100), 0.5),
    sphere(vect(1,-1,9), 0.5, vect(80,0,100), 0.5),
    sphere(vect(1,-1,10), 0.5, vect(60,0,100), 0.5),
};

/* ============================================================================
 * CONFIG
 * ============================================================================
 */

struct config {
    int x_size,
        y_size,
        num_rays,
        max_bounces
        ;
    float lens_f,
        focal_len,
        aperture_size
        ;
    std::string out_fn;
    vect sky_color;
};

static config m_conf = {
    /*.x_size =        */ 512,
    /*.y_size =        */ 512,
    /*.num_rays =      */ 5,
    /*.max_bounces =   */ 5,
    /*.lens_f =        */ 50.0f,
    /*.focal_len =     */ 50.0f,
    /*.aperture_size = */ 0.005f,
    /*.out_fn =        */ std::string("out.ppm")
};

#define STR_TO_PROP(st, pr) else if(prop.compare(st) == 0){ in >> conf->pr; }

#define PROPS_STRS(M) \
    M("X_SIZE", x_size) \
    M("Y_SIZE", y_size) \
    M("NUM_RAYS", num_rays) \
    M("MAX_BOUNCES", max_bounces) \
    M("FOCAL_LEN_m", focal_len) \
    M("LENS_F", lens_f) \
    M("APERTURE_SIZE_m", aperture_size) \
    M("OUT_FN", out_fn)

static int load_config(const char* fn, config* conf){
    std::ifstream in(fn);
    if ( ! in.is_open()) {
        LOG_ERROR("Could not open config file");
        return (-1);
    }
    int sky_r, sky_g, sky_b;
    for(std::string prop; in >> prop;) {
        if(0) ;
        PROPS_STRS(STR_TO_PROP)
        else if(prop.compare("SKY_R") == 0) in >> sky_r;
        else if(prop.compare("SKY_G") == 0) in >> sky_g;
        else if(prop.compare("SKY_B") == 0) in >> sky_b;
    }
    conf->sky_color = vect(sky_r, sky_g, sky_b);
    
    return 0;
}

/* ============================================================================
 * Helper functions
 * ============================================================================
 */

// random float
static float frand() { return (float) rand() / RAND_MAX; }

// random sampling from the aperture circle
static vect rand_aperture() {
    float r = frand() * m_conf.aperture_size, t = frand() * 2 * M_PI;
    return vect(r * cos(t), r*sin(t), m_conf.focal_len);
}

static ray_t lens(vect dir, vect origin) {
    // lens distance from vector origin
    float dist = (m_conf.focal_len - origin.z);

    // point of incidence on the lens
    vect c = vect(origin.x+(dir.x / dir.z)*dist, origin.y + (dir.y/dir.z)*dist, 0);
    if(m_debug) LOG("c: %f, %f, %f", c.x, c.y, c.z);

    // focal plane calculation
    // tldr: take incidence-point to center and compare it with the focal length
    vect y = dir * ((m_conf.lens_f) / dir.z) - c;

    // finally, set the proper z-coordinate for the outgoing ray
    // this matters since we're not pretending the camera is 0-size
    // this also assumes that camera will always face z-forward in this stage
    c.z = m_conf.focal_len;

    ray_t ret;
    ret.dir    = y;
    ret.origin = c;
    return ret;

}

// Sample scene point with direction and origin
vect sample_scene(vect dir, vect origin, int n) {
    // normalize
    dir = !dir;
    /* calculate intersection */
    scene_obj *nearest_intesect = NULL;
    float nearest_dist = FLT_MAX;
    for (int i = 0; i < ARR_LEN(scene); i++) {
        float d = scene[i].intersect(dir, origin);
        if (d > 0 && d < nearest_dist) {
            nearest_intesect = &scene[i];
            nearest_dist = d;
        }
    }
    if (!nearest_intesect) return SKY_COLOR;
    vect col = nearest_intesect->col_rgb * (1-nearest_intesect->reflect);
    vect ix_pt = origin+(dir*nearest_dist);
    vect norm = ix_pt - nearest_intesect->loc;
    vect hv = dir + norm*(norm%dir*-2);
    // TODO: scene_obj should return its normal, nicer for when I don't just have spheres

    // smol optimization possible: if reflect is 0 don't do this
    if (n<m_conf.max_bounces){
        col += sample_scene(!hv, ix_pt, n+1)*nearest_intesect->reflect;
    }
    return col;
}

vect get_pixel(int x, int y) {
    // pixel location on sensor (0,0) is center, size is (1,1)
    vect px_l = vect( ((float) x) / m_conf.x_size - 0.5, ((float)y) / m_conf.y_size - 0.5,0) * 0.035;

    vect col = vect(0,0,0); // pixel color, will be aggregated

    for (int i = m_conf.num_rays /*TODO: cos^4 rule*/; i--; ) {
        vect dir = !(rand_aperture() - px_l); // photon direction (from pixel)
        
        // lens distortion is needed for depth-of-field
        ray_t r = lens(dir, px_l);

        // TODO: camera rotation
        //       Currently it is assumed camera will point z-forward
        //       Could keep all other functions the same and just apply a rotation at this point
        //       this keeps the lens math simple and only looks at cam_dir in this stage
        // r = rotate(r, cam_dir, cam_pos);

        col += sample_scene(r.dir, r.origin + cam_pos, 0)/m_conf.num_rays;
    }

    // clamp, not truly needed
    if(col.x > 255) col.x = 255;
    if(col.y > 255) col.y = 255;
    if(col.z > 255) col.z = 255;
    if(col.x < 0)col.x = 0;
    if(col.y < 0)col.y = 0;
    if(col.z < 0)col.z = 0;
    return col;
}

int main(int argc, char** argv)
{
    if (argc > 1 && strcmp(argv[1], "test") == 0) {
        load_config("in.txt", &m_conf);
        m_debug = true;

        LOG("Test center ray: expected 0.2,0.2,focal_len");
        vect ray = vect(0.2,0.2,m_conf.focal_len);
        vect org = vect(-0.2,-0.2, 0.0);
        ray_t r = lens(ray, org);
        // undo scaling done by lens
        r.dir = r.dir * m_conf.focal_len / m_conf.lens_f;
        LOG("r: %f, %f, %f", r.dir.x,r.dir.y,r.dir.z);
        
        LOG("Test f-point ray");
        ray = vect(0.2,0.2,0.5);
        org = vect(0.0,0.0,(m_conf.focal_len - m_conf.lens_f));
        LOG("org: %f, %f, %f", org.x,org.y,org.z);
        r = lens(ray, org);
        LOG("r: %f, %f, %f", r.dir.x,r.dir.y,r.dir.z);

        return 0;
    }
    if (argc > 1) load_config(argv[1], &m_conf);
    const char* fn = m_conf.out_fn.c_str();
    FILE* f = fopen(fn, "wb");
    if (!f) {
        LOG_ERROR("Unable to open output file!\n");
        return -1;
    }
    LOG("output: %s\n", fn);
    fprintf(f, "P6 %d %d 255 ", m_conf.x_size, m_conf.y_size);
    for (int y = 0; y < m_conf.y_size; y++) {
        printf("%d\n",y);
        for (int x = 0; x < m_conf.x_size; x++) {
            vect p = get_pixel(x,y);
            int cx = std::lround(p.x);
            int cy = std::lround(p.y);
            int cz = std::lround(p.z);
            fprintf(f, "%c%c%c", cx,cy,cz);
        }
    }
    fclose(f);
}