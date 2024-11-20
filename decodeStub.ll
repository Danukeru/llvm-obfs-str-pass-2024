; ModuleID = 'hello.bc'
source_filename = "decodeStub.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) uwtable
define dso_local void @decode(ptr noundef %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = icmp ne ptr %0, null
  %4 = icmp sgt i32 %1, 0
  %5 = and i1 %3, %4
  br i1 %5, label %6, label %14

6:                                                ; preds = %2, %6
  %7 = phi ptr [ %10, %6 ], [ %0, %2 ]
  %8 = phi i32 [ %9, %6 ], [ %1, %2 ]
  %9 = add nsw i32 %8, -1
  %10 = getelementptr inbounds i8, ptr %7, i64 1
  %11 = load i8, ptr %7, align 1, !tbaa !5
  %12 = xor i8 %11, -81
  store i8 %12, ptr %7, align 1, !tbaa !5
  %13 = icmp sgt i32 %8, 1
  br i1 %13, label %6, label %14, !llvm.loop !8

14:                                               ; preds = %6, %2
  ret void
}

attributes #0 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Debian clang version 18.1.8 (12)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"omnipotent char", !7, i64 0}
!7 = !{!"Simple C/C++ TBAA"}
!8 = distinct !{!8, !9, !10}
!9 = !{!"llvm.loop.mustprogress"}
!10 = !{!"llvm.loop.unroll.disable"}
