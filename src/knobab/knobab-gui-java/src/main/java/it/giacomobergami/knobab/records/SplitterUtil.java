package it.giacomobergami.knobab.records;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

public abstract class SplitterUtil<T> {

    List<T> objects;
    String options[];
    String[] headers;
    private Class<T> clazz;

    public SplitterUtil(String x, Class<T> tClass) {
        clazz = tClass;
        objects = new ArrayList<>();
        options = null;
        headers = null;
        if (!x.isEmpty()) {
            options = x.split("\\r?\\n");
            headers = options[0].split(",");
        }
    }

    private void loadAllUpTo(int rowLimit) {
        for (int i = objects.size()+1, M = Math.min(options.length, rowLimit+1); i<=M; i++) {
            T obj = null;
            try {
                obj = clazz.getConstructor().newInstance();
                String[] row = options[i].split(",");
                for (int j = 0, N  = Math.min(headers.length, row.length); j<N; j++) {
                    Field f = null;
                    try {
                        f = clazz.getField(headers[j]);
                        try {
                            f.set(obj, f.getType().getMethod("valueOf", String.class).invoke(null, row[j]));
                        } catch (InvocationTargetException | IllegalAccessException | NoSuchMethodException e) {
                            f.set(obj, row[j]);
                        }
                    } catch (NoSuchFieldException e) {
                        e.printStackTrace();
                    }

                }
                objects.add(obj);
            } catch (IllegalAccessException | InvocationTargetException | NoSuchMethodException | InstantiationException e) {
                e.printStackTrace();
            }
        }
    }

    protected T get(int rowId) {
        loadAllUpTo(rowId);
        return objects.get(rowId);
    }

    public abstract Object cell(int row, int col);
    public int size() {
        return options.length-1;
    }
}
