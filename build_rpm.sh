# Build an RPM for the device driver
# The package will rely on DKMS for compilation/installation

CURRENT_DIR=`pwd`
RPM_ROOTDIR=$CURRENT_DIR/build/RPM
SPEC_FILE=serialfc-dkms.spec

# Cleanup
rm -rf $RPM_ROOTDIR

# Create directory structure
mkdir -p $RPM_ROOTDIR/TMP
mkdir -p $RPM_ROOTDIR/BUILD
mkdir -p $RPM_ROOTDIR/RPMS
mkdir -p $RPM_ROOTDIR/SOURCES
mkdir -p $RPM_ROOTDIR/SPECS
mkdir -p $RPM_ROOTDIR/SRPMS

# Produce a tarball of the git repository
git archive -o $RPM_ROOTDIR/SOURCES/serialfc-linux.tar HEAD

# Start RPM build
rpmbuild -bb -v --define="_topdir ${RPM_ROOTDIR}" ${SPEC_FILE}
