#include "HM/RotationMap.hpp"

vfps::RotationMap::RotationMap(PhaseSpace* in, PhaseSpace* out,
							   const size_t xsize, const size_t ysize,
							   const meshaxis_t angle,
							   const ROTATION_TYPE mn) :
	HeritageMap(in,out,xsize,ysize)
{
	std::vector<interpol_t> ti;
	ti.resize(_xsize*_ysize);

	const meshaxis_t cos_dt = cos(angle);
	const meshaxis_t sin_dt = -sin(angle);

	std::array<unsigned int,12> num;
	num.fill(0);
	for(unsigned int p_i=0; p_i< _ysize; p_i++) {
		for (unsigned int q_i=0; q_i< _xsize; q_i++) {

			// Cell of inverse image (qp,pp) of grid point i,j.
			meshaxis_t qp; //q', backward mapping
			meshaxis_t pp; //p'
			// interpolation type specific q and p coordinates
			meshaxis_t pcoord;
			meshaxis_t qcoord;
			meshaxis_t qq_int;
			meshaxis_t qp_int;
			//Scaled arguments of interpolation functions:
			unsigned int id; //meshpoint smaller q'
			unsigned int jd; //numper of lower mesh point from p'
			interpol_t xiq; //distance from id
			interpol_t xip; //distance of p' from lower mesh point
			switch (mn) {
			case ROTATION_TYPE::MESH:
				qp = cos_dt*(q_i-_xsize/2.0)
						- sin_dt*(p_i-_ysize/2.0)+_xsize/2.0;
				pp = sin_dt*(q_i-_xsize/2.0)
						+ cos_dt*(p_i-_ysize/2.0)+_ysize/2.0;
				qcoord = qp;
				pcoord = pp;
				break;
			case ROTATION_TYPE::NORMAL:
				qp = cos_dt*((q_i-_xsize/2.0)/_xsize)
						- sin_dt*((p_i-_ysize/2.0)/_ysize);
				pp = sin_dt*((q_i-_xsize/2.0)/_xsize)
						+ cos_dt*((p_i-_ysize/2.0)/_ysize);
				qcoord = (qp+0.5)*_xsize;
				pcoord = (pp+0.5)*_ysize;
				break;
			case ROTATION_TYPE::NORMAL2:
				qp = cos_dt*(2*static_cast<int>(q_i)-static_cast<int>(_xsize))
						/static_cast<int>(_xsize)
				   - sin_dt*(2*static_cast<int>(p_i)-static_cast<int>(_ysize))
						/static_cast<int>(_ysize);

				pp = sin_dt*(2*static_cast<int>(q_i)-static_cast<int>(_xsize))
						/static_cast<int>(_xsize)
				   + cos_dt*(2*static_cast<int>(p_i)-static_cast<int>(_ysize))
						/static_cast<int>(_ysize);
				qcoord = (qp+1)*_xsize/2;
				pcoord = (pp+1)*_ysize/2;
				break;
			}
			xiq = std::modf(qcoord, &qq_int);
			xip = std::modf(pcoord, &qp_int);
			id = qq_int;
			jd = qp_int;

			if (id <  _xsize && jd < _ysize)
			{
				// gridpoint matrix used for interpolation
				std::array<std::array<hi,it>,it> ph;

				// arrays of interpolation coefficients
				std::array<interpol_t,it> icq;
				std::array<interpol_t,it> icp;

				std::array<interpol_t,it*it> hmc;

				// create vectors containing interpolation coefficiants
				switch (it)
				{
				case INTERPOL_TYPE::NONE:
					icq[0] = 1;

					icp[0] = 1;
					break;

				case INTERPOL_TYPE::LINEAR:
					icq[0] = interpol_t(1)-xiq;
					icq[1] = xiq;

					icp[0] = interpol_t(1)-xip;
					icp[1] = xip;
					break;

				case INTERPOL_TYPE::QUADRATIC:
					icq[0] = xiq*(xiq-interpol_t(1))/interpol_t(2);
					icq[1] = interpol_t(1)-xiq*xiq;
					icq[2] = xiq*(xiq+interpol_t(1))/interpol_t(2);

					icp[0] = xip*(xip-interpol_t(1))/interpol_t(2);
					icp[1] = interpol_t(1)-xip*xip;
					icp[2] = xip*(xip+interpol_t(1))/interpol_t(2);
					break;

				case INTERPOL_TYPE::CUBIC:
					icq[0] = (xiq-interpol_t(1))*(xiq-interpol_t(2))*xiq
							* interpol_t(-1./6.);
					icq[1] = (xiq+interpol_t(1))*(xiq-interpol_t(1))
							* (xiq-interpol_t(2)) / interpol_t(2);
					icq[2] = (interpol_t(2)-xiq)*xiq*(xiq+interpol_t(1))
							/ interpol_t(2);
					icq[3] = xiq*(xiq+interpol_t(1))*(xiq-interpol_t(1))
							* interpol_t(1./6.);

					icp[0] = (xip-interpol_t(1))*(xip-interpol_t(2))*xip
							* interpol_t(-1./6.);
					icp[1] = (xip+interpol_t(1))*(xip-interpol_t(1))
							* (xip-interpol_t(2)) / interpol_t(2);
					icp[2] = (interpol_t(2)-xip)*xip*(xip+interpol_t(1))
							/ interpol_t(2);
					icp[3] = xip*(xip+interpol_t(1))*(xip-interpol_t(1))
							* interpol_t(1./6.);
					break;
				}

				//  Assemble interpolation
				for (size_t hmq=0; hmq<it; hmq++) {
					for (size_t hmp=0; hmp<it; hmp++){
						hmc[hmp*it+hmq] = icq[hmp]*icp[hmq];
					}
				}


				// renormlize to minimize rounding errors
//				renormalize(hmc.size(),hmc.data());

				// write heritage map
				for (unsigned int j1=0; j1<it; j1++) {
					unsigned int j0 = jd+j1-(it-1)/2;
					for (unsigned int i1=0; i1<it; i1++) {
						unsigned int i0 = id+i1-(it-1)/2;
						if(i0< _xsize && j0 < _ysize ){
							ph[i1][j1].index = i0*_ysize+j0;
							ph[i1][j1].index2d = {{i0,j0}};
							ph[i1][j1].weight = hmc[i1*it+j1];
						} else {
							ph[i1][j1].index = 0;
							ph[i1][j1].index2d = {{0,0}};
							ph[i1][j1].weight = 0;
						}
						_heritage_map[q_i][p_i][i1*it+j1]
								= ph[i1][j1];
					}
				}
			}
		}
	}
	#ifdef FR_USE_CL
	__initOpenCL();
	#endif
}