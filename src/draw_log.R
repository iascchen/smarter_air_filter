setwd("/Users/chenhao/workspaces/arduino/smarter_air_filter/data");

library("scales");
library("ggplot2");
library("Cairo");
library("showtext");

date_str <- c("2014-05-11" , "2014-05-12" , "2014-05-13");

log_data <- read.csv("pm_with_filter_4.csv" ,header=TRUE);

log_data$dia <- as.POSIXct(log_data$Time, format="%Y-%m-%d %H:%M:%S ");
log_data$Fan = ifelse(log_data$Status > 0, "开", "关");
head(log_data);

CairoPNG("0511.png", 1000, 300);
showtext.begin();

ggplot(log_data, aes(x=dia, y=PM, fill=factor(Fan))) +
	geom_bar(stat="identity") +
	xlab("时间") +
	scale_fill_manual(values = c("开"="red", "关"="light green")) +
	scale_x_datetime(breaks = date_breaks("2 hour"),
                    labels = date_format("%m-%dH%H"),
					limits = c(as.POSIXct("2014-05-11"), 
                    	as.POSIXct("2014-05-12")) );

showtext.end();
dev.off();
