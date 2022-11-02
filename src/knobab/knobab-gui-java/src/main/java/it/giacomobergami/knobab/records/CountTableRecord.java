package it.giacomobergami.knobab.records;


public class CountTableRecord  {

    public Long Act;

    public Long TraceId;

    public Long Count;

    public CountTableRecord() {
        this(0,0,0);
    }

    public CountTableRecord(long act, long traceId, long count) {
        Act = act;
        TraceId = traceId;
        Count = count;
    }

    public long getAct() {
        return Act;
    }

    public void setAct(long act) {
        Act = act;
    }

    public long getTraceId() {
        return TraceId;
    }

    public void setTraceId(long traceId) {
        TraceId = traceId;
    }

    public long getCount() {
        return Count;
    }

    public void setCount(long count) {
        Count = count;
    }
}
