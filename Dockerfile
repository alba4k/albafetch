FROM ubuntu
RUN apt-get update && apt-get install make gcc -y
WORKDIR /albafetch
COPY src/* /
COPY Makefile .
CMD ["ls"]
# CMD [ "make", "run" ]
