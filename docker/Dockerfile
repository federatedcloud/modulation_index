#Acts as a modification to run Laura Spitler's version of single-pulse searching
FROM nanograv/pulsar

USER jovyan

#pull and build PRESTO modifications

ENV PSRHOME=/opt/pulsar
ENV PRESTO=$PSRHOME/presto 
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PRESTO/lib 
ENV PYTHONPATH=$PYTHONPATH:$PRESTO/lib/python
ENV PATH=$PATH:$PRESTO/bin

WORKDIR $PSRHOME

RUN mkdir PALFA && \
	mkdir PALFA/bin
	
ENV PALFA=$PSRHOME/PALFA

WORKDIR $PALFA

RUN git clone https://github.com/federatedcloud/transients_pipeline2.git

WORKDIR $PRESTO/src

RUN make clean

WORKDIR /home/jovyan

RUN git clone https://github.com/federatedcloud/modulation_index.git && \
	cd modulation_index && \
	cp changes/*.h $PRESTO/include && \
	cp changes/*.c $PRESTO/src

WORKDIR $PRESTO/src

RUN make prep && \
    make

WORKDIR $PRESTO/python

RUN /opt/conda/envs/python2/bin/python setup.py install --home=$PRESTO 

RUN cd /home/jovyan/modulation_index/mi_src && \
	gcc -Wall palfa_calc_mi.c -o palfa_mi -lm && \
	cp palfa_mi $PALFA/bin

COPY .bashrc /home/jovyan/.bashrc
COPY .profile /home/jovyan/.profile
	
	
#RUN echo "export PALFA=$PSRHOME/PALFA" >> /home/jovyan/.bashrc
#RUN echo "export PATH=/opt/conda/bin:$PATH" >> /home/jovyan/.bashrc
	
USER root


RUN groupadd -g 999 docker
RUN usermod -a -G docker jovyan


